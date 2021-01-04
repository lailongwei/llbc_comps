#pragma once

#include "IDB.h"
#include "comp_com/Common.h"

class LLBC_COMP_EXPORT IDBMgr : public LLBC_IFacade
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

