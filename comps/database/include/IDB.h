#ifndef _IDB_H_
#define _IDB_H_

#include <string>
#include <functional>

enum class MODE
{
    MODE_NONE,
    MODE_READ,  //只读
    MODE_EDIT   //读写
};

class IRecord
{
public:
    virtual ~IRecord() = default;

    /**
     * 通过数据位置索引获取值
     */
    virtual int64_t GetInt(uint32_t idx) const = 0;
    virtual double GetDouble(uint32_t idx) const = 0;
    virtual const std::string &GetStr(uint32_t idx) const = 0;
    /**
     * 通过数据域名称获取值
     */
    virtual int64_t GetInt(const char *name) const = 0;
    virtual double GetDouble(const char *name) const = 0;
    virtual const std::string &GetStr(const char *name) const = 0;

    virtual void SetInt(uint32_t idx, int64_t val) = 0;
    virtual void SetDouble(uint32_t idx, double val) = 0;
    virtual void SetStr(uint32_t idx, const char *val) = 0;
    virtual void SetBlob(uint32_t nIdx, const char *val, uint32_t len) = 0;

    virtual void SetInt(const char *name, int64_t val) = 0;
    virtual void SetDouble(const char *name, double val) = 0;
    virtual void SetStr(const char *name, const char *val) = 0;
    virtual void SetBlob(const char *name, const char *val, uint32_t len) = 0;
};

/**
 * 查询结果集合
 */
class IRecordset
{
public:
    virtual ~IRecordset() = default;

    //从集合充获取一条记录(集合仍然管理Record对象内存)
    virtual IRecord *GetRecord(uint32_t idx) = 0;

    //从集合从获取一条记录(集合不再管理Record对象内存)
    virtual IRecord *PopRecord(uint32_t idx) = 0;

    //获得集合记录数量
    virtual uint32_t CountRecord() const = 0;

    virtual IRecord &operator[](uint32_t idx) = 0;
};

/**
 * 异步执行sql回调
 */
using DBAsyncRecordCB = std::function<void(IRecord *)>;
using DBAsyncRecordsetCB = std::function<void(IRecordset *)>;
using DBAsyncSqlCB = std::function<void(bool)>;

/**
 * 数据库对象接口
 */
class IDatabase
{
public:
    virtual ~IDatabase() = default;

    // 初始化database
    virtual bool Init(const std::string &ip, int port, const std::string &user, const std::string &passwd, const std::string &dbName,
                      int asyncConnNum) = 0;
    // 销毁数据库连接
    virtual void Destroy() = 0;

    // 等待所有连接刷新未处理的请求
    virtual void Flush() = 0;

    // 定时调用，处理异步任务回调
    virtual void OnUpdate() = 0;

public:
    // 同步查询单条记录
    virtual IRecord *QueryRecord(const char *sql, MODE mode) = 0;

    // 同步查询多条记录
    virtual IRecordset *QueryRecordset(const char *sql, MODE mode) = 0;

    // 同步执行sql
    virtual bool Query(const char *sql) = 0;

public:
    // 异步查询单条记录
    virtual void QueryRecordAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordCB cb) = 0;

    // 异步查询多条记录
    virtual void QueryRecordsetAsync(uint64_t key, const char *sql, MODE mode, DBAsyncRecordsetCB cb) = 0;

    // 异步执行sql
    virtual void QueryAsync(uint64_t key, const char *sql, DBAsyncSqlCB cb) = 0;

public:
    // 同步创建指定表默认记录
    virtual IRecord *MakeDefRecord(const char *tableName) = 0;

    // 同步插入记录
    virtual bool InsertRecord(IRecord *record) = 0;

    // 同步更新记录
    virtual bool UpdateRecord(IRecord *record) = 0;

    // 同步删除记录
    virtual bool DeleteRecord(IRecord *record) = 0;
};

extern "C"  IDatabase *CreateDB(const std::string &ip, int port, const std::string &user, const std::string &passwd,
                                           const std::string &dbName,
                                int asyncConnNum);

#endif
