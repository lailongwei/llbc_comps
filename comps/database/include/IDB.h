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

enum class MODE
{
    MODE_NONE,
    MODE_READ,  //read only
    MODE_EDIT   //read and write
};

/**
* Database init param.
*/
struct LLBC_COMP_EXPORT DatabaseParam final
{
    DatabaseParam(const LLBC_String& name,
                  const LLBC_String &ip, 
                  int port, 
                  const LLBC_String &user, 
                  const LLBC_String &passwd, 
                  const LLBC_String &dbName,
                  int asyncConnNum)
        : _name(name)
        , _ip(ip)
        , _port(port)
        , _user(user)
        , _passwd(passwd)
        , _dbName(dbName)
        , _asyncConnNum(asyncConnNum)
    {}

    LLBC_String _name;
    LLBC_String _ip;
    int _port;
    LLBC_String _user;
    LLBC_String _passwd;
    LLBC_String _dbName;
    int _asyncConnNum;
};

/**
* Database single record object.
*/
class LLBC_COMP_EXPORT IRecord
{
public:
    virtual ~IRecord() = default;

public:
    virtual sint64 GetInt(uint32 idx) const = 0;
    virtual double GetDouble(uint32 idx) const = 0;
    virtual const LLBC_String &GetStr(uint32 idx) const = 0;

    virtual sint64 GetInt(const LLBC_String& name) const = 0;
    virtual double GetDouble(const LLBC_String& name) const = 0;
    virtual const LLBC_String &GetStr(const LLBC_String& name) const = 0;

public:
    virtual void SetInt(uint32 idx, sint64 val) = 0;
    virtual void SetDouble(uint32 idx, double val) = 0;
    virtual void SetStr(uint32 idx, const char *val) = 0;
    virtual void SetBlob(uint32 nIdx, const char *val, uint32 len) = 0;

    virtual void SetInt(const LLBC_String& name, sint64 val) = 0;
    virtual void SetDouble(const LLBC_String& name, double val) = 0;
    virtual void SetStr(const LLBC_String& name, const char *val) = 0;
    virtual void SetBlob(const LLBC_String& name, const char *val, uint32 len) = 0;
};

/**
 * Database records set
 */
class LLBC_COMP_EXPORT IRecordset
{
public:
    virtual ~IRecordset() = default;

public:
    // Get a record from set. 
    virtual IRecord *GetRecord(uint32 idx) = 0;

    //Pop a record from set.(release record)
    virtual IRecord *PopRecord(uint32 idx) = 0;

    //Get record count.
    virtual uint32 GetSize() const = 0;

    // Get a record ref.
    virtual IRecord &operator[](uint32 idx) = 0;
};

/**
 * async query callback.
 */
using AsyncQueryCB = std::function<void(IRecordset *)>;
/**
* async execute sql callback.
*/
using AsyncSqlCB = std::function<void(bool)>;

/**
 * Database object.
 */
class LLBC_COMP_EXPORT IDatabase
{
public:
    virtual ~IDatabase() = default;

public:
    // flush all async query.
    virtual void Flush() = 0;

    // get connection name.
    virtual const LLBC_String &GetName() const = 0;

public:
    // synchronize query.
    virtual IRecordset *Query(const char *sql, MODE mode) = 0;

    // synchronize execute sql.
    virtual bool Query(const char *sql) = 0;

    // asynchronous query.
    virtual void QueryAsync(uint64 key, const char *sql, MODE mode, AsyncQueryCB cb) = 0;

    // asynchronous execute sql.
    virtual void QueryAsync(uint64 key, const char *sql, AsyncSqlCB cb) = 0;
};
