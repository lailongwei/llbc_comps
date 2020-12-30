#ifndef _IDB_MGR_H_
#define _IDB_MGR_H_

#include "IDB.h"
#include "llbc/comm/IFacade.h"

using namespace llbc;

class IDBMgr : public LLBC_IFacade
{
public:
    IDBMgr()
        : LLBC_IFacade(LLBC_FacadeEvents::DefaultEvents | LLBC_FacadeEvents::OnUpdate)
    {}
    virtual ~IDBMgr() = default;

public:
    virtual void Flush() = 0;

    virtual IDatabase *GetDatabase(const LLBC_String &dbConnName) = 0;
    virtual IDatabase *GetDefaultDatabase() = 0;
};

#endif // _IDB_MGR_H_
