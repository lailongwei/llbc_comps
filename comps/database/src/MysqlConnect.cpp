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

#include "MysqlConnect.h"
#include "Database.h"
#include "Record.h"
#include "Recordset.h"
#include "FieldFactory.h"

#include "llbc/core/thread/Guard.h"
#include "llbc/core/log/Log.h"

//mysql query warning log time.
const static int __query_warning_time = 100;

MysqlConnect::MysqlConnect(MysqlDB &db, const DatabaseParam &cfg)
    : _db(db)
    , _cfg(cfg)
    , _lastErrorCode(0)
{}

MysqlConnect::~MysqlConnect()
{
    Disconnect();
}

bool MysqlConnect::Connect()
{
    MYSQL *hdbc = mysql_init(nullptr);
    if (!hdbc)
        return false;

    if (!mysql_real_connect(hdbc, 
                            _cfg._ip.c_str(), 
                            _cfg._user.c_str(), 
                            _cfg._passwd.c_str(), 
                            _cfg._dbName.c_str(), 
                            _cfg._port, 
                            nullptr, 
                            0))
    {
        this->SetLastError();
        Log.e2<MysqlConnect>("could not connect to mysql. name[%s]errno[%d] errMsg[%s]",
                             _cfg._name.c_str(),
                             GetLastErrorNo(),
                             GetLastError().c_str());
        mysql_close(hdbc);
        return false;
    }

    char value = 1;
    mysql_options(hdbc, MYSQL_OPT_RECONNECT, (char *) &value);
    mysql_options(hdbc, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    _opened = true;
    _dbHandler = hdbc;
    return true;
}

bool MysqlConnect::Ping()
{
    if (!_dbHandler)
        return false;

    if (mysql_ping(_dbHandler))
        return false;

    return true;
}

bool MysqlConnect::IsConnect()
{
    return _opened;
}

void MysqlConnect::Disconnect()
{
    if (_opened && _dbHandler)
    {
        mysql_close(_dbHandler);
        _opened = false;
    }
}

MYSQL *MysqlConnect::GetHandler()
{
    return _dbHandler;
}

sint32 MysqlConnect::GetLastErrorNo()
{
    return _lastErrorCode;
}

const LLBC_String& MysqlConnect::GetLastError() const
{
    return _lastError;
}

Record *MysqlConnect::CreateRecord(MYSQL_ROW row, 
                                   MYSQL_FIELD *dbFields, 
                                   const unsigned long *colLens, 
                                   uint32 fieldNum, 
                                   MODE mode)
{
    std::unique_ptr<Record> newRec(new Record(fieldNum, LLBC_String(dbFields[0].org_table), mode));

    for (uint32 i = 0; i < fieldNum; i++)
    {
        const MYSQL_FIELD &dbField = dbFields[i];
        const DBFieldInfo *fieldInfo = _db.QueryDBFieldInfo(dbField);

        if (fieldInfo == nullptr)
        {
            Log.e2<MysqlConnect>("mysql create fieldInfo failed. name[%s]", _cfg._name.c_str());
            return nullptr;
        }

        if (dbField.flags & PRI_KEY_FLAG)
            newRec->SetKeyIdx(i);

        if (dbField.flags & AUTO_INCREMENT_FLAG)
            newRec->SetAutoIncIdx(i);

        BaseField *baseField = FieldFactory::CreateField(*fieldInfo);
        if (baseField == nullptr)
            return nullptr;

        baseField->SetValue(row ? row[i] : dbField.def, colLens ? colLens[i] : dbField.max_length);
        newRec->WriteField(i, baseField);
    }

    return newRec.release();
}

// 同步查询多条记录
IRecordset *MysqlConnect::Query(const char *sql, MODE mode)
{
    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
        return nullptr;

    // auto free MYSQL_RES
    std::shared_ptr<void> __resGuard((void *) 0, [res](void *) { mysql_free_result(res); });
    mysql_data_seek(res, 0);

    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32 fieldNum = mysql_num_fields(res);

    std::unique_ptr<Recordset> recSet(new Recordset(static_cast<uint32>(mysql_num_rows(res))));

    MYSQL_ROW row;
    uint32 idx = 0;
    while (row = mysql_fetch_row(res))
    {
        Record *newRec = CreateRecord(row, dbFields, colLens, fieldNum, mode);
        if (!newRec)
            return nullptr;

        recSet->WriteRecord(idx++, newRec);
    }

    assert(idx == recSet->GetSize());
    return recSet.release();
}

// 同步执行sql
bool MysqlConnect::Query(const char *sql)
{
    return Query(sql, nullptr);
}

void MysqlConnect::SetLastError()
{
    _lastErrorCode = mysql_errno(_dbHandler);
    _lastError = mysql_error(_dbHandler);
}

bool MysqlConnect::Query(const char *sql, MYSQL_RES **res)
{
    if (!IsConnect())
    {
        Log.e2<MysqlConnect>("mysql isn't connected, could not query. name[%s]", _cfg._name.c_str());
        return false;
    }

    try
    {
        const sint64 startTimeInMs = LLBC_GetMilliSeconds();

        std::shared_ptr<void> __timerGuard((void *) 0, [sql, startTimeInMs](void *) {
            // query overtime log.
            const sint64 endTimeInMs = LLBC_GetMilliSeconds();
            if (endTimeInMs - startTimeInMs > __query_warning_time)
                Log.w2<MysqlConnect>("mysql query overtime. sql[%s] time[%lld]", sql, endTimeInMs - startTimeInMs);
        });

        int ret = mysql_real_query(_dbHandler, sql, (int) strlen(sql));
        if (ret != 0)
        {
            this->SetLastError();
            Log.e2<MysqlConnect>("mysql query fail. name[%s] sql[%s] errno[%d] errMsg[%s]", 
                                 _cfg._name.c_str(), 
                                 sql, 
                                 GetLastErrorNo(), 
                                 GetLastError().c_str());
            return false;
        }

        if (res)
            *res = mysql_store_result(_dbHandler);
    }
    catch (const std::exception &)
    {
        this->SetLastError();
        Log.e2<MysqlConnect>("mysql query crash. name[%s] sql[%s] errno[%d] errMsg[%s]", 
                             _cfg._name.c_str(), 
                             sql, 
                             GetLastErrorNo(), 
                             GetLastError().c_str());
        return false;
    }
    return true;
}

IRecord *MysqlConnect::MakeDefRecord(const char *tableName)
{
    char sql[256] = { 0 };
    snprintf(sql, sizeof(sql) - 1, "select * from %s limit 0", tableName);

    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
    {
        Log.e2<MysqlConnect>("mysql def record res is null. name[%s] table[%s] errno[%d] errMsg[%s]", 
                             _cfg._name.c_str(), 
                             tableName, 
                             GetLastErrorNo(), 
                             GetLastError().c_str());
        return nullptr;
    }

    // auto free MYSQL_RES
    std::shared_ptr<void> __resGuard((void *) 0, [res](void *) { mysql_free_result(res); });

    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
    {
        Log.e2<MysqlConnect>("mysql def record field is null. name[%s] table[%s] errno[%d] errMsg[%s]",
                             _cfg._name.c_str(),
                             tableName,
                             GetLastErrorNo(),
                             GetLastError().c_str());
        return nullptr;
    }

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32 fieldNum = mysql_num_fields(res);

    return CreateRecord(nullptr, dbFields, colLens, fieldNum, MODE::MODE_EDIT);
}
