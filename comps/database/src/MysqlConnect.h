#pragma once

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
    MysqlConnect(MysqlDB &db, const LLBC_String &ip, int port, const LLBC_String &user, const LLBC_String &passwd, const LLBC_String &dbName);
    ~MysqlConnect();

public:
    bool Connect();
    bool Ping();
    bool IsConnect();
    void Disconnect();

    //get mysql handler
    MYSQL *GetHandler();

    //get mysql last error.
    uint32 GetLastErrorNo();
    const LLBC_String GetLastError();

public:
    // synchronize query.
    IRecordset *Query(const char *sql, MODE mode);

    // synchronize execute sql.
    bool Query(const char *sql);

    // create a default record by tableName
    IRecord *MakeDefRecord(const char *tableName);

private:  
    // synchronize query and get result
    bool Query(const char *sql, MYSQL_RES **res);

    // create record from mysql table row.
    Record *CreateRecord(MYSQL_ROW row, 
                         MYSQL_FIELD *dbFields, 
                         const unsigned long *colLens, 
                         uint32 fieldNum, 
                         MODE mode);

    // set mysq last error.
    void SetLastError();

private:
    MysqlDB &_db;

    LLBC_String _ip;
    int32_t _port;
    LLBC_String _user;
    LLBC_String _pwd;
    LLBC_String _dbName;

    uint32 _lastErrorCode;
    LLBC_String _lastError;

    bool _opened = false;
    MYSQL *_dbHandler = nullptr;
};

