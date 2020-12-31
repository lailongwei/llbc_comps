#pragma once

#include <mysql_version.h>
#include "mysql.h"
#include <mutex>
#include <vector>
#include <map>
#include <memory>

#include "AsyncTask.h"
#include "AsyncTaskQueue.h"

class MysqlConnect;
struct DBFieldInfo;

class MysqlDB : public IDatabase
{
public:
    MysqlDB();

   // Init database.
    virtual bool Init(const LLBC_String &ip, 
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

