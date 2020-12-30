#ifndef _MYSQL_CONNECT_H_
#define _MYSQL_CONNECT_H_

#include <mysql_version.h>
#include <mysql.h>

#include "llbc/common/BasicDataType.h"
#include "llbc/common/StringDataType.h"

using namespace llbc;

class IRecord;
class IRecordset;
class Record;
class MysqlDB;
enum class MODE;

class MysqlConnect
{
public:
    MysqlConnect(MysqlDB &db, const std::string &ip, int port, const std::string &user, const std::string &passwd, const std::string &dbName);
    ~MysqlConnect();

public:
    bool Connect();
    bool Ping();
    bool IsConnect();
    void Disconnect();

    MYSQL *GetHandler();
    uint32_t GetLastErrorNo();
    const std::string GetLastError();

public:
    // 同步查询单条记录
    IRecord *QueryRecord(const char *sql, MODE mode);

    // 同步查询多条记录
    IRecordset *QueryRecordset(const char *sql, MODE mode);

    // 同步执行sql
    bool Query(const char *sql);

    // 创建指定表默认记录
    IRecord *MakeDefRecord(const char *tableName);

private:  
    // 同步执行sql并返回结果集
    bool Query(const char *sql, MYSQL_RES **res);

    Record *CreateRecord(MYSQL_ROW row, MYSQL_FIELD *dbFields, const unsigned long *colLens, uint32_t fieldNum, MODE mode);

    void SetError();

private:
    MysqlDB &_db;

    std::string _ip;
    int32_t _port;
    std::string _user;
    std::string _pwd;
    std::string _dbName;

    uint32_t _lastErrorCode;
    std::string _lastError;

    bool _opened = false;
    MYSQL *_dbHandler = nullptr;
};

#endif
