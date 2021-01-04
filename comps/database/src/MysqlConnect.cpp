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

MysqlConnect::MysqlConnect(MysqlDB &db, 
                           const LLBC_String &ip, 
                           int port, 
                           const LLBC_String &user, 
                           const LLBC_String &passwd,
                           const LLBC_String &dbName)
    : _db(db)
    , _ip(ip)
    , _port(port)
    , _user(user)
    , _pwd(passwd)
    , _dbName(dbName)
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

    if (!mysql_real_connect(hdbc, _ip.c_str(), _user.c_str(), _pwd.c_str(), _dbName.c_str(), _port, nullptr, 0))
    {
        this->SetLastError();
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

uint32 MysqlConnect::GetLastErrorNo()
{
    return _lastErrorCode;
}

const LLBC_String MysqlConnect::GetLastError()
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
            return nullptr;

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

    //函数结束自动释放MYSQL_RES
    std::shared_ptr<void> _resGuard((void *) 0, [res](void *) { mysql_free_result(res); });
    mysql_data_seek(res, 0);

    // 取字段信息
    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32 fieldNum = mysql_num_fields(res);

    std::unique_ptr<Recordset> recSet = std::make_unique<Recordset>(static_cast<uint32>(mysql_num_rows(res)));

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
        return false;

    try
    {
        int ret = mysql_real_query(_dbHandler, sql, (int) strlen(sql));
        if (ret != 0)
        {
            this->SetLastError();
            return false;
        }

        if (res)
            *res = mysql_store_result(_dbHandler);
    }
    catch (const std::exception &)
    {
        this->SetLastError();
        return false;
    }
    return true;
}

// 创建指定表默认记录
IRecord *MysqlConnect::MakeDefRecord(const char *tableName)
{
    char sql[256] = { 0 };
    snprintf(sql, sizeof(sql) - 1, "select * from %s limit 0", tableName);

    MYSQL_RES *res = nullptr;
    if (!Query(sql, &res))
        return nullptr;

    if (!res)
        return nullptr;

    //函数结束自动释放MYSQL_RES
    std::shared_ptr<void> _resGuard((void *) 0, [res](void *) { mysql_free_result(res); });

    // 取字段信息
    MYSQL_FIELD *dbFields = mysql_fetch_fields(res);
    if (!dbFields)
        return nullptr;

    const unsigned long *colLens = mysql_fetch_lengths(res);
    const uint32 fieldNum = mysql_num_fields(res);

    return CreateRecord(nullptr, dbFields, colLens, fieldNum, MODE::MODE_EDIT);
}
