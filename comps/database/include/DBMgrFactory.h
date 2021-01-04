#pragma once

#include "comp_com/Common.h"

class LLBC_COMP_EXPORT DBMgrFactory : public LLBC_IFacadeFactory
{
public:
    virtual LLBC_IFacade *Create() const;
};

