#ifndef _Database_H_
#define _Database_H_

#include <mysql_version.h>
#include "mysql.h"
#include <mutex>
#include <vector>
#include <map>
#include <memory>

#include "IDB.h"
#include "AsyncTask.h"
#include "AsyncTaskQueue.h"

class MysqlConnect;
struct DBFieldInfo;


class MysqlDB : public IDatabase
{
public:
    MysqlDB();

    // 初始化database
    virtual bool Init(const std::string &ip, int port, const std::string &user, const std::string &passwd, const std::string &dbName,
                      int acyncConnNum) override;
    // 销毁数据库连接
    virtual void Destroy() override;

    // 等待所有连接刷新未处理的请求
    virtual void Flush() override;

    // 定时调用，处理异步任务回调
    virtual void OnUpdate() override;

public:
    // 同步查询单条记录
    virtual IRecord *QueryRecord(const char *sql, MODE mode) override;

    // 同步查询多条记录
    virtual IRecordset *QueryRecordset(const char *sql, MODE mode) override;

    // 同步执行sql
    virtual bool Query(const char *sql) override;

public:
    // 异步查询单条记录
    virtual void QueryRecordAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordCB cb) override;

    // 异步查询多条记录
    virtual void QueryRecordsetAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordsetCB cb) override;

    // 异步执行sql
    virtual void QueryAsync(uint64_t key, const char *sql, DBAsyncSqlCB cb) override;

public:
    //保存域信息
    const DBFieldInfo *QueryDBFieldInfo(const MYSQL_FIELD &field);

    // 同步创建指定表默认记录
    virtual IRecord *MakeDefRecord(const char *tableName) override;

    // 同步插入记录
    virtual bool InsertRecord(IRecord *record) override;

    // 同步更新记录
    virtual bool UpdateRecord(IRecord *record) override;

    // 同步删除记录
    virtual bool DeleteRecord(IRecord *record) override;

private:
    // 添加异步任务
    void AddAsyncTask(uint64_t key, AsyncTask *task);

    // 异步线程函数
    void AsyncWorkFunc(uint32_t threadIdx);

    // 执行异步任务
    void AsyncDoWork(MysqlConnect *conn, AsyncTask *task);

private:
    bool _inited;
    std::thread::id _asyncThreadId;
    uint32_t _asyncConnNum;
    
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
    std::map<std::string, DBFieldInfo *> _fieldInfos;
};

#endif  // !_Database_H_
