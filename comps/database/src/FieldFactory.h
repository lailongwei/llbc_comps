#pragma once

#include "BaseField.h"

class FieldFactory
{
public:
    static BaseField *CreateField(const DBFieldInfo &info);
};

