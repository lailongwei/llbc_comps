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

#include "comp_com/Common.h"

class IRecord;
class IDatabase;

class LLBC_COMP_EXPORT RecordManipulator
{
public:
    // sync create a default record.
    static IRecord *MakeDefRecord(IDatabase* db, const char *tableName);

public:
    // sync insert record.
    static bool Insert(IDatabase *db, IRecord *record);

    // sync update record.
    static bool Update(IDatabase *db, IRecord *record);

    // sync delete record.
    static bool Delete(IDatabase *db, IRecord *record);

public:
    // build update record sql.
    static bool BuildUpdateSql(IDatabase *db, IRecord *record, LLBC_String &sql);

    // build delete record sql.
    static bool BuildDelSql(IDatabase *db, IRecord *record, LLBC_String &sql);

    // build insert record sql.
    static bool BuildInsertSql(IDatabase *db, IRecord *record, LLBC_String &sql);
};
