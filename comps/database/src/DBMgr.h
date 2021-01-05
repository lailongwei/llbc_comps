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

#include "IDBMgr.h"
#include "Database.h"
#include <memory>
#include <map>

class DBMgr : public IDBMgr
{
public:
    DBMgr() = default;
    virtual ~DBMgr() = default;

public:
    virtual bool OnInitialize() override;
    virtual void OnDestroy() override;
    virtual bool OnStart() override;
    virtual void OnStop() override;
    virtual void OnUpdate() override;

public:
    virtual void Flush() override;

    virtual IDatabase *GetDatabase(const LLBC_String &dbConnName) override;
    virtual IDatabase *GetDefaultDatabase() override;

private:
    MysqlDB *CreateDatabase(const LLBC_String& name,
                            const LLBC_String &ip, 
                            int port, 
                            const LLBC_String &user, 
                            const LLBC_String &passwd, 
                            const LLBC_String &dbName,
                            int asyncConnNum);

private:
    MysqlDB *_defaultDB = nullptr;
    std::map<LLBC_String, std::unique_ptr<MysqlDB>> _databases;
};
