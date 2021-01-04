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

#include "SqlUtil.h"
#include "Record.h"
#include "BaseField.h"
#include <mysql.h>
#include <sstream>
#include <list>
#include <assert.h>

const static int __DefaultNum2StrLen = 20;

#define IS_FIELD_NUM(x)                                                                                                                  \
    ((x) == FIELD_TYPE_TINY || (x) == FIELD_TYPE_YEAR || (x) == FIELD_TYPE_SHORT || (x) == FIELD_TYPE_INT24 || (x) == FIELD_TYPE_LONG || \
     (x) == FIELD_TYPE_LONGLONG || (x) == FIELD_TYPE_FLOAT || (x) == FIELD_TYPE_DOUBLE)

#define IS_FIELD_BLOB(x) ((x) == FIELD_TYPE_TINY_BLOB || (x) == FIELD_TYPE_MEDIUM_BLOB || (x) == FIELD_TYPE_LONG_BLOB || (x) == FIELD_TYPE_BLOB)

bool SqlUtil::BuildCondition(MYSQL *handle, Record *record, LLBC_String &cond)
{
    int32_t keyIdx = record->GetAutoIncIdx();
    if (keyIdx < 0)
        keyIdx = record->GetKeyIdx();

    if (keyIdx < 0 && keyIdx >= record->GetFieldNum())
        return false;

    auto keyField = record->GetField(keyIdx);
    auto keyName = keyField->GetName();
    auto keyType = keyField->GetType();

    char sqlBuf[1024] = { 0 };
    if (IS_FIELD_NUM(keyType))
    {
        snprintf(sqlBuf, sizeof(sqlBuf) - 1, "`%s`=%lld", keyName, record->GetInt(keyIdx));
    }
    else
    {
        auto &keyVal = record->GetStr(keyIdx);
        snprintf(sqlBuf, sizeof(sqlBuf) - 1, "`%s`='%s'", keyName, keyVal.c_str());
    }
    cond = sqlBuf;
    return true;
}

void SqlUtil::BuildFormatSql(MYSQL *handle, const BaseField *field, char *format, int formatSize)
{
    switch (field->GetType())
    {
    case FIELD_TYPE_STRING:
    case FIELD_TYPE_VAR_STRING:
    case FIELD_TYPE_DATE:
    case FIELD_TYPE_DATETIME:
    {
        if (field->GetStr().empty())
        {
            snprintf(format, formatSize, "`%s`=\'\'", field->GetName());
        }
        else
        {
            const auto bufSize = field->GetStr().size();
            char *sqlBuf = new char[bufSize * 2]();

            mysql_escape_string(sqlBuf, field->GetStr().c_str(), bufSize);
            snprintf(format, formatSize, "`%s`='%s'", field->GetName(), sqlBuf);
        }
    }
    break;
    case FIELD_TYPE_DOUBLE:
    case FIELD_TYPE_FLOAT:
    {
        snprintf(format, formatSize, "`%s`=%f", field->GetName(), field->GetDouble());
    }
    break;
    case FIELD_TYPE_TINY:
    {
        if ((field->GetAttr() & UNSIGNED_FLAG) != 0)
            snprintf(format, formatSize, "`%s`=%lu", field->GetName(), (DWORD) field->GetInt());
        else
            snprintf(format, formatSize, "`%s`=%d", field->GetName(), (int) field->GetInt());
    }
    break;
    case FIELD_TYPE_SHORT:
    {
        if ((field->GetAttr() & UNSIGNED_FLAG) != 0)
            snprintf(format, formatSize, "`%s`=%lu", field->GetName(), (DWORD) field->GetInt());
        else
            snprintf(format, formatSize, "`%s`=%d", field->GetName(), (int) field->GetInt());
    }
    break;
    case FIELD_TYPE_LONG:
    {
        if ((field->GetAttr() & UNSIGNED_FLAG) != 0)
            snprintf(format, formatSize, "`%s`=%lu", field->GetName(), (unsigned long) field->GetInt());
        else
            snprintf(format, formatSize, "`%s`=%ld", field->GetName(), (long) field->GetInt());
    }
    break;
    case FIELD_TYPE_LONGLONG:
    {
        if ((field->GetAttr() & UNSIGNED_FLAG) != 0)
            snprintf(format, formatSize, "`%s`=%llu", field->GetName(), uint64(field->GetInt()));
        else
            snprintf(format, formatSize, "`%s`=%lld", field->GetName(), field->GetInt());
    }
    break;
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
    {
        snprintf(format, formatSize, "`%s`='", field->GetName());
        char *end = format + strlen(format);
        end += mysql_real_escape_string(handle, end, field->GetStr().c_str(), field->GetStr().size());
        *end++ = '\'';
        *end++ = '\0';
    }
    break;
    default:
        assert(false);
        break;
    }
}

bool SqlUtil::BuildOperation(MYSQL *handle, Record *record, LLBC_String &cond)
{
    int32_t keyIdx = record->GetAutoIncIdx();
    if (keyIdx < 0)
        keyIdx = record->GetKeyIdx();

    const auto fieldNUm = record->GetFieldNum();
    if (keyIdx < 0 && keyIdx >= fieldNUm)
        return false;

    int32_t maxFeildStrLen = 0;

    std::list<const BaseField *> updateFields;
    for (int32_t idx = 0; idx < fieldNUm; ++idx)
    {
        const BaseField *field = record->GetField(idx);
        if (!field->IsChanged())
            continue;

        auto fieldType = field->GetType();
        if (IS_FIELD_NUM(fieldType) && maxFeildStrLen == 0)
            maxFeildStrLen = __DefaultNum2StrLen;
        else if (field->GetLen() > maxFeildStrLen)
            maxFeildStrLen = field->GetLen()*2;

        updateFields.push_back(field);
    }

    if (updateFields.empty())
        return true;

    maxFeildStrLen += 128;
    std::vector<char> __buf(maxFeildStrLen);
    char *sqlBuf = (char *) &__buf[0];

    bool first = true;
    std::stringstream strFormat;
    for (auto field : updateFields)
    {
        memset(sqlBuf, 0x0, maxFeildStrLen);
        BuildFormatSql(handle, field, sqlBuf, maxFeildStrLen);

        if (!first)
            strFormat << "," << sqlBuf;
        else
        {
            first = false;
            strFormat << sqlBuf;
        }
    }
    cond = strFormat.str();
    return true;
}

bool SqlUtil::BuildUpdateSql(MYSQL *handle, Record *record, LLBC_String &sql)
{
    LLBC_String cond;
    if (!BuildCondition(handle, record, cond))
        return false;

    LLBC_String op;
    if (!BuildOperation(handle, record, op))
        return false;

    if (op.empty())
        return true;
    
    auto &tableNam = record->GetTableName();

    const auto buffSize = cond.size() + op.size() + tableNam.size() + 128;
    char *sqlBuf = new char[buffSize]();
    snprintf(sqlBuf, buffSize, "update `%s` set %s where %s limit 1", tableNam.c_str(), op.c_str(), cond.c_str());

    sql = sqlBuf;
    delete[] sqlBuf;
    return true;
}

bool SqlUtil::BuildDelSql(MYSQL *handle, Record *record, LLBC_String &sql)
{
    LLBC_String cond;
    if (!BuildCondition(handle, record, cond))
        return false;

    char sqlBuf[1024] = { 0 };
    snprintf(sqlBuf, sizeof(sqlBuf) - 1, "delete from `%s` where %s limit 1", record->GetTableName().c_str(), cond.c_str());

    sql = sqlBuf;
    return true;
}

bool SqlUtil::BuildInsertSql(MYSQL *handle, Record *record, LLBC_String &sql)
{
    LLBC_String op;
    if (!BuildOperation(handle, record, op))
        return false;

    if (op.empty())
        return false;

    auto &tableNam = record->GetTableName();

    const auto buffSize = op.size() + tableNam.size() + 128;
    char *sqlBuf = new char[buffSize]();
    snprintf(sqlBuf, buffSize, "insert into `%s` set %s", tableNam.c_str(), op.c_str());

    sql = sqlBuf;
    delete[] sqlBuf;
    return true;
}
