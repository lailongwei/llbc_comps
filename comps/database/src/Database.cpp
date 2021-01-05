// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "Database.h"
#include "MysqlConnect.h"
#include <algorithm>
#include "SqlUtil.h"
#include "BaseField.h"
#include "Record.h"

#include "llbc/core/log/Log.h"

//校验同步操作是否是在同步线程调用
#define CHECK_ASYNC_THREAD assert(_asyncThreadId == std::this_thread::get_id());
//sleep milliseconds time
#define DB_SLEEP(time) std::this_thread::sleep_for(std::chrono::milliseconds(time))

MysqlDB::MysqlDB()
    : _inited(false)
    , _stopedNum(0)
    , _stoping(false)
    , _asyncConnNum(0)
    , _flushing(false)
{}

bool MysqlDB::Init(const LLBC_String &name, 
                   const LLBC_String &ip, 
                   int port, 
                   const LLBC_String &user, 
                   const LLBC_String &passwd,
                   const LLBC_String &dbName,
                   int acyncConnNum)
{
    if (_inited)
    {
        Log.e2<MysqlDB>("Database name[%s] already init.", name.c_str());
        return false;
    }

    Log.i2<MysqlDB>("Init database. name[%s]", name.c_str());

    _syncConn.reset(new MysqlConnect(*this, name, ip, port, user, passwd, dbName));
    if (!_syncConn->Connect())
    {
        Log.e2<MysqlDB>("Failed to connect to database. name[%s]", name.c_str());
        return false;
    }

    _asyncConnNum = std::max<int>(acyncConnNum, 1);

    for (uint32 i = 0; i < _asyncConnNum; ++i)
    {
        std::unique_ptr<MysqlConnect> conn(new MysqlConnect(*this, name, ip, port, user, passwd, dbName));
        if (!conn->Connect())
        {
            Log.e2<MysqlDB>("Failed to connect to database. name[%s]", name.c_str());
            return false;
        }

        _asyncConns.emplace_back(std::move(conn));
    }

    _flushFlag.reserve(_asyncConnNum);
    _taskQueues.reserve(_asyncConnNum);
    _asyncThreads.reserve(_asyncConnNum);

    for (uint32 i = 0; i < _asyncConnNum; ++i)
    {
        _flushFlag.emplace_back(std::unique_ptr<std::atomic_bool>(new std::atomic_bool));
        _taskQueues.emplace_back(std::unique_ptr<AsyncTaskQueue>(new AsyncTaskQueue()));
        _asyncThreads.emplace_back(std::unique_ptr<std::thread>(new std::thread (&MysqlDB::AsyncWorkFunc, this, i)));
    }

    _asyncThreadId = std::this_thread::get_id();

    _name = name;
    _inited = true;
    return true;
}

void MysqlDB::Destroy()
{
    CHECK_ASYNC_THREAD

    Log.i2<MysqlDB>("Destroy database. name[%s]", _name.c_str());

    if (!_inited || _stoping)
    {
        Log.e2<MysqlDB>("Database isn't init, could not destroy. name[%s]", _name.c_str());
        return;
    }

    this->Flush();

    _stoping = true;
    _stopedNum = 0;
    while (_stopedNum != _asyncConnNum) 
        DB_SLEEP(100);

    //等待所有异步线程退出
    for (auto &th : _asyncThreads)
    {
        if (th->joinable())
            th->join();
    }
    _asyncThreads.clear();

    while (!_finishTasks.empty())
    {
        auto task = _finishTasks.front();
        _finishTasks.pop();
        delete task;
    }

    _flushFlag.clear();
    _syncConn.reset();
    _asyncConns.clear();
    _taskQueues.clear();
    _flushFlag.clear();

    for (auto &fieldData : _fieldInfos) 
        delete fieldData.second;

    _fieldInfos.clear();

    _asyncConnNum = 0;
    _inited = false;
}

void MysqlDB::Flush()
{
    CHECK_ASYNC_THREAD

    Log.i2<MysqlDB>("Flush database. name[%s]", _name.c_str());

    if (!_inited || _flushing)
    {
        Log.e2<MysqlDB>("Database isn't init, could not flush. name[%s]", _name.c_str());
        return;
    }

    for (uint32 i = 0; i < _asyncConnNum; ++i) 
        *_flushFlag[i] = false;

    _flushing = true;
    for (uint32 i = 0; i < _asyncConnNum; ++i)
    {
        auto& flushFlag = _flushFlag[i];
        while (!(*flushFlag)) 
            DB_SLEEP(100);
    }

    _flushing = false;
}

void MysqlDB::OnUpdate() 
{
    CHECK_ASYNC_THREAD

    _finishLock.lock();
    if (_finishTasks.empty())
    {
        _finishLock.unlock();
        return;
    }

    std::queue<AsyncTask *> tasks;
    _finishTasks.swap(tasks);
    _finishLock.unlock();

    while (!tasks.empty())
    {
        auto task = tasks.front();
        tasks.pop();

        task->Invoke();
        delete task;
    }
}

IRecordset *MysqlDB::Query(const char *sql, MODE mode)
{
    CHECK_ASYNC_THREAD
    return _syncConn->Query(sql, mode);
}

bool MysqlDB::Query(const char *sql)
{
    CHECK_ASYNC_THREAD
    return _syncConn->Query(sql);
}

void MysqlDB::QueryAsync(uint64 key, const char *sql, MODE mode, AsyncQueryCB cb)
{
    TaskQuery *task = new TaskQuery();
    task->taskType = AsyncTaskType::Query;
    task->sql = sql;
    task->task.mode = mode;
    task->task.cb = std::move(cb);

    this->AddAsyncTask(key, task);
}

void MysqlDB::QueryAsync(uint64 key, const char *sql, AsyncSqlCB cb)
{
    TaskQuerySql *task = new TaskQuerySql();
    task->taskType = AsyncTaskType::ExecuteSql;
    task->sql = sql;
    task->task.cb = std::move(cb);

    this->AddAsyncTask(key, task);
}

const DBFieldInfo *MysqlDB::QueryDBFieldInfo(const MYSQL_FIELD &field)
{
    if (field.org_name == nullptr || field.org_table == nullptr)
        return nullptr;

    char key[1024] = { 0 };
    snprintf(key, sizeof(key) - 1, "%s.%s", field.org_table, field.org_name);

    LLBC_String findKey(key);
    std::transform(findKey.begin(), findKey.end(), findKey.begin(), ::tolower);

    std::lock_guard<std::mutex> l(_fieldLock);
    auto it = _fieldInfos.find(findKey);
    if (it != _fieldInfos.end())
        return it->second;

    DBFieldInfo *info = new DBFieldInfo();
    info->name = field.org_name;
    info->type = field.type;
    info->attr = field.flags;
    info->len = field.length;
    _fieldInfos.insert(std::make_pair(findKey, info));
    return info;
}

IRecord *MysqlDB::MakeDefRecord(const char *tableName)
{
    return _syncConn->MakeDefRecord(tableName);
}

bool MysqlDB::BuildUpdateSql(IRecord *record, LLBC_String &sql)
{
    return SqlUtil::BuildUpdateSql(_syncConn->GetHandler(), (Record *) record, sql);
}

bool MysqlDB::BuildDelSql(IRecord *record, LLBC_String &sql)
{
    return SqlUtil::BuildDelSql(_syncConn->GetHandler(), (Record *) record, sql);
}

bool MysqlDB::BuildInsertSql(IRecord *record, LLBC_String &sql)
{
    return SqlUtil::BuildInsertSql(_syncConn->GetHandler(), (Record *) record, sql);
}

bool MysqlDB::Insert(IRecord *record)
{
    LLBC_String sql;
    if (!BuildInsertSql(record, sql))
        return false;

    return _syncConn->Query(sql.c_str());
}

bool MysqlDB::Update(IRecord *record)
{
    auto updateRec = (Record *) record;
    if (updateRec->GetMode() != MODE::MODE_EDIT)
        return false;

    LLBC_String sql;
    if (!BuildUpdateSql(record, sql))
        return false;

    // 可能没有可更新字段
    if (sql.empty())
        return true;

    bool ret = _syncConn->Query(sql.c_str());
    if (ret)
        updateRec->ClsEditFlag();

    return ret;
}

bool MysqlDB::Delete(IRecord *record)
{
    auto delRec = (Record *) record;
    if (delRec->GetMode() != MODE::MODE_EDIT)
        return false;

    LLBC_String sql;
    if (!BuildDelSql(record, sql))
        return false;

    return _syncConn->Query(sql.c_str());
}

void MysqlDB::AddAsyncTask(uint64 key, AsyncTask *task)
{
    CHECK_ASYNC_THREAD
    auto &taskQueue = _taskQueues[key % _asyncConnNum];
    taskQueue->Push(task);
}

void MysqlDB::AsyncWorkFunc(uint32 threadIdx)
{
    MysqlConnect *conn = _asyncConns[threadIdx].get();
    auto &taskQueue = _taskQueues[threadIdx];
    auto& flushFlag = _flushFlag[threadIdx];

    std::vector<AsyncTask *> tasks;
    while (true)
    {
        tasks.clear();
        taskQueue->TimePopAll(tasks, 50);

        for (auto task : tasks)
        {
            this->AsyncDoWork(conn, task);
        }

        if (_flushing && taskQueue->Empty())
            *flushFlag = true;
        else if (_stoping)
            break;
    }

    ++_stopedNum;
}

void MysqlDB::AsyncDoWork(MysqlConnect *conn, AsyncTask *task)
{
    switch (task->taskType)
    {
    case AsyncTaskType::Query:
    {
        TaskQuery *t = dynamic_cast<TaskQuery *>(task);
        t->task.result = conn->Query(task->sql.c_str(), t->task.mode);
    }
    break;
    case AsyncTaskType::ExecuteSql:
    {
        TaskQuerySql *t = dynamic_cast<TaskQuerySql *>(task);
        t->task.result = conn->Query(task->sql.c_str());
    }
    break;
    default:
        break;
    }
    _finishLock.lock();
    _finishTasks.push(task);
    _finishLock.unlock();
}
