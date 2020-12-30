#ifndef _FieldFactory_H_
#define _FieldFactory_H_
#include "BaseField.h"

class FieldFactory
{
public:
    static BaseField *CreateField(const DBFieldInfo &info);
};

#endif
