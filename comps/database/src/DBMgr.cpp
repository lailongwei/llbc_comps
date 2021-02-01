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
#include "llbc/core/log/Log.h"

bool DBMgr::OnInitialize()
{
    const auto& dbCfg = LLBC_IApplication::ThisApp()->GetIniConfig();
    const auto dbNum = dbCfg.GetValue<uint32>("Database", "num");
    if (dbNum == 0)
    {
        Log.i2<DBMgr>("Database config is zero.");
        return true;
    }

    for (uint32 i = 1; i <= dbNum; ++i)
    {
        LLBC_String cfgName;
        cfgName.format("Database%d", i);
        auto dbSession = dbCfg.GetSection(cfgName);
        if (!dbSession)
        {
            Log.e2<DBMgr>("Can't found database config. [%s]", cfgName.c_str());
            return false;
        }

        const auto name = dbSession->GetValue<LLBC_String>("name");
        if (_databases.find(name) != _databases.end())
        {
            Log.e2<DBMgr>("Database name is repeated. name[%s]", name.c_str());
            return false;
        }

        auto newDB = CreateMysqlDB(DatabaseParam(name,
                                                 dbSession->GetValue<LLBC_String>("ip"),
                                                 dbSession->GetValue<uint32>("port"),
                                                 dbSession->GetValue<LLBC_String>("user"),
                                                 dbSession->GetValue<LLBC_String>("passwd"),
                                                 dbSession->GetValue<LLBC_String>("db"),
                                                 dbSession->GetValue<uint32>("connect")));
        if (!newDB)
        {
            Log.e2<DBMgr>("Failed to create database. name[%s]", name.c_str());
            return false;
        }

        if (dbSession->GetValue<uint32>("default") > 0)
        {
            if (_defaultDB)
            {
                Log.e2<DBMgr>("Default database is repeated. name[%s]", name.c_str());
                return false;
            }
            _defaultDB = newDB;
        }

        _databases[name].reset(newDB);
    }

    if (!_defaultDB)
    {
        Log.e2<DBMgr>("Default database is null.");
        return false;
    }
    
    return true;
}

void DBMgr::OnDestroy()
{
    for (auto& db : _databases)
    {
        if (db.second)
            db.second->Destroy();
    }
    _databases.clear();
}

bool DBMgr::OnStart()
{
    Log.i2<DBMgr>("DBMgr OnStart.");
    return true;
}

void DBMgr::OnStop()
{
    Log.i2<DBMgr>("DBMgr OnStop.");
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

void DBMgr::DestroyDatabase(const LLBC_String &dbConnName)
{
    auto it = _databases.find(dbConnName);
    if (it != _databases.end())
    {
        Log.e2<DBMgr>("Database isn't exists. name[%s]", dbConnName.c_str());
        return ;
    }

    it->second->Destroy();
    _databases.erase(it);
}

IDatabase *DBMgr::CreateDatabase(const DatabaseParam &cfg)
{
    if (_databases.find(cfg._name) != _databases.end())
    {
        Log.e2<DBMgr>("Database name is repeated. name[%s]", cfg._name.c_str());
        return nullptr;
    }

    auto newDB = CreateMysqlDB(cfg);
    if (!newDB)
    {
        Log.e2<DBMgr>("Failed to create database. name[%s]", cfg._name.c_str());
        return nullptr;
    }

    _databases[cfg._name].reset(newDB);
    return newDB;
}

MysqlDB *DBMgr::CreateMysqlDB(const DatabaseParam &cfg)
{
    MysqlDB *db = new MysqlDB();
    if (!db->Init(cfg))
    {
        Log.e2<DBMgr>("Failed to create MysqlDB. name[%s]", cfg._name.c_str());
        delete db;
        return nullptr;
    }
    return db;
}