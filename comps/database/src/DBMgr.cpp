#include "DBMgr.h"
#include "IDB.h"

bool DBMgr::OnInitialize()
{
    //��������
    const char *ip = "127.0.0.1";
    const uint32 port = 33123;
    const char *name = "llbc";
    const char *pwd = "llbc123456";
    const char *db = "llbc";

    _defaultDB = CreateDatabase(ip, port, name, pwd, db, 1);
    _databases["llbc"].reset(_defaultDB);
    
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