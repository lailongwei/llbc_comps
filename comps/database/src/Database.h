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

#pragma once

#include <mysql/mysql.h>
#include <mutex>
#include <vector>
#include <map>
#include <memory>
#include <thread>

#include "AsyncTask.h"
#include "AsyncTaskQueue.h"
#include "MysqlConnect.h"

struct DBFieldInfo;

class MysqlDB : public IDatabase
{
public:
    MysqlDB();

   // Init database.
    virtual bool Init(const LLBC_String& name,
                      const LLBC_String &ip, 
                      int port, 
                      const LLBC_String &user, 
                      const LLBC_String &passwd, 
                      const LLBC_String &dbName,
                      int asyncConnNum) override;

    // destroy database.
    virtual void Destroy() override;

    // flush all async query.
    virtual void Flush() override;

    // 定时调用，处理异步任务回调
    void OnUpdate();

public:
    // synchronize query.
    virtual IRecordset *Query(const char *sql, MODE mode) override;

    // synchronize execute sql.
    virtual bool Query(const char *sql) override;

    // asynchronous query.
    virtual void QueryAsync(uint64 key, const char *sql, MODE mode, AsyncQueryCB cb) override;

    // asynchronous execute sql.
    virtual void QueryAsync(uint64 key, const char *sql, AsyncSqlCB cb) override;

 public:
    IRecord *MakeDefRecord(const char *tableName);

    // build update record sql.
    bool BuildUpdateSql(IRecord *record, LLBC_String &sql);

    // build delete record sql.
    bool BuildDelSql(IRecord *record, LLBC_String &sql);

    // build insert record sql.
    bool BuildInsertSql(IRecord *record, LLBC_String &sql);

    // sync insert record.
    bool Insert(IRecord *record);

    // sync update record.
    bool Update(IRecord *record);

    // sync delete record.
    bool Delete(IRecord *record);

public:
    // save mysql field info.
    const DBFieldInfo *QueryDBFieldInfo(const MYSQL_FIELD &field);

private:
    // add async task to queue.
    void AddAsyncTask(uint64 key, AsyncTask *task);

    // async work function.
    void AsyncWorkFunc(uint32 threadIdx);

    // do async work.
    void AsyncDoWork(MysqlConnect *conn, AsyncTask *task);

private:
    bool _inited;
    std::thread::id _asyncThreadId;
    uint32 _asyncConnNum;
    LLBC_String _name;
    
    std::atomic_bool _stoping; //结束标记
    std::atomic_bool _flushing; //刷新标记
    std::atomic_int _stopedNum; //已停止异步线程数量

    std::unique_ptr<MysqlConnect> _syncConn;
    std::vector<std::unique_ptr<MysqlConnect>> _asyncConns;
    std::vector<std::unique_ptr<std::thread>> _asyncThreads;
    std::vector<std::unique_ptr<AsyncTaskQueue>> _taskQueues;
    std::vector<std::unique_ptr<std::atomic_bool>> _flushFlag;

    std::mutex _finishLock;
    std::queue<AsyncTask *> _finishTasks;

    std::mutex _fieldLock;
    std::map<LLBC_String, DBFieldInfo *> _fieldInfos;
};

