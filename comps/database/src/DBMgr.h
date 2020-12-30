#ifndef _DB_MGR_H_
#define _DB_MGR_H_

#include "IDBMgr.h"
#include "IDB.h"
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
    IDatabase* _defaultDB = nullptr;
    std::map<LLBC_String, std::unique_ptr<IDatabase>> _databases;
};

#endif  // _DB_MGR_H_
