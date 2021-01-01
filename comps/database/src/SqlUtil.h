#pragma once

#include <string>
#include <mysql.h>
#include "llbc/common/StringDataType.h"

using namespace llbc;

class Record;
class BaseField;

class SqlUtil
{
public:
    // build update record sql.
    static bool BuildUpdateSql(MYSQL *handle, Record *record, LLBC_String &sql);

    // build delete record sql.
    static bool BuildDelSql(MYSQL *handle, Record *record, LLBC_String &sql);

    // build insert record sql.
    static bool BuildInsertSql(MYSQL *handle, Record *record, LLBC_String &sql);

private:
    static bool BuildCondition(MYSQL *handle, Record *record, LLBC_String &cond);
    static bool BuildOperation(MYSQL *handle, Record *record, LLBC_String &cond);
    static void BuildFormatSql(MYSQL *handle, const BaseField *field, char *format, int formatSize);
};
