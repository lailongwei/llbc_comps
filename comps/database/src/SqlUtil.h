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
