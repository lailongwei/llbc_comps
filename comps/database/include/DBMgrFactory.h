#pragma once

#include "llbc/comm/IFacade.h"

using namespace llbc;

class DBMgrFactory : public LLBC_IFacadeFactory
{
public:
    virtual LLBC_IFacade *Create() const;
};

