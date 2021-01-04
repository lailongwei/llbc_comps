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

#include "DBMgr.h"
#include "IDB.h"

bool DBMgr::OnInitialize()
{
    const auto& dbCfg = LLBC_IApplication::ThisApp()->GetIniConfig();
    auto dbSession = dbCfg.GetSection("DefaultDB");
    if (!dbSession)
        return false;

    const auto ip = dbSession->GetValue<LLBC_String>("ip");
    const auto port = dbSession->GetValue<uint32>("port");
    const auto name = dbSession->GetValue<LLBC_String>("name");
    const auto pwd = dbSession->GetValue<LLBC_String>("passwd");
    const auto db = dbSession->GetValue<LLBC_String>("db");
    const auto connectNum = dbSession->GetValue<uint32>("connect");

    _defaultDB = CreateDatabase(ip, port, name, pwd, db, connectNum);
    if (!_defaultDB)
        return false;

    _databases["DefaultDB"].reset(_defaultDB);
    return true;
}

void DBMgr::OnDestroy()
{
    // do nothing
}

bool DBMgr::OnStart()
{
    return true;
}

void DBMgr::OnStop()
{
    // do nothing
}

void DBMgr::OnUpdate()
{
   for (auto& db : _databases)
       db.second->OnUpdate();
}

void DBMgr::Flush()
{
    for (auto &db : _databases) 
        db.second->Flush();
}

IDatabase *DBMgr::GetDatabase(const LLBC_String &dbConnName)
{
    if (_databases.find(dbConnName) != _databases.end())
        return _databases[dbConnName].get();

    return nullptr;
}

IDatabase *DBMgr::GetDefaultDatabase()
{
    return _defaultDB;
}

MysqlDB *DBMgr::CreateDatabase(const LLBC_String &ip, int port, const LLBC_String &user, const LLBC_String &passwd, const LLBC_String &dbName,
                          int asyncConnNum)
{
    MysqlDB *db = new MysqlDB();
    if (!db->Init(ip, port, user, passwd, dbName, asyncConnNum))
    {
        delete db;
        return nullptr;
    }
    return db;
}