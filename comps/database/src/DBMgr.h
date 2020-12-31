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
    MysqlDB *CreateDatabase(const LLBC_String &ip, int port, const LLBC_String &user, const LLBC_String &passwd, const LLBC_String &dbName,
                              int asyncConnNum);

private:
    MysqlDB *_defaultDB = nullptr;
    std::map<LLBC_String, std::unique_ptr<MysqlDB>> _databases;
};
