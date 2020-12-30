#ifndef _DBMGR_FACTORY_H_
#define _DBMGR_FACTORY_H_

#include "llbc/comm/IFacade.h"

using namespace llbc;

class DBMgrFactory : public LLBC_IFacadeFactory
{
public:
    virtual LLBC_IFacade *Create() const;
};

#endif // _DBMGR_FACTORY_H_
