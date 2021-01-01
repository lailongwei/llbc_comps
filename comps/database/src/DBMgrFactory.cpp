#include "DBMgrFactory.h"
#include "DBMgr.h"

LLBC_IFacade *DBMgrFactory::Create() const
{
    return new DBMgr();
}
