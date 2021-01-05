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
    sint32 _port;
    LLBC_String _user;
    LLBC_String _pwd;
    LLBC_String _dbName;

    uint32 _lastErrorCode;
    LLBC_String _lastError;

    bool _opened = false;
    MYSQL *_dbHandler = nullptr;
};

