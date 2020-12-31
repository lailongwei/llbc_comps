#pragma once

#include "llbc/common/StringDataType.h"
using namespace llbc;

struct DBFieldInfo
{
    LLBC_String name;
    uint32 type = 0;
    uint32 attr = 0;
    uint32 len = 0;
};

class BaseField
{
public:
    explicit BaseField(const DBFieldInfo &rInfo)
        : _info(rInfo)
    {}

    virtual ~BaseField() = default;

public:
    inline bool IsChanged(void) const
    {
        return _changed;
    }
    inline void TagChanged(bool changed)
    {
        _changed = changed;
    }
    inline const char *GetName(void) const
    {
        return _info.name.c_str();
    }
    inline uint32 GetType(void) const
    {
        return _info.type;
    }
    inline uint32 GetAttr(void) const
    {
        return _info.attr;
    }
    inline uint32 GetLen(void) const
    {
        return _info.len;
    }

public:
    virtual bool SetValue(const char *value, uint32 len) = 0;

    virtual sint64 GetInt() const = 0;
    virtual double GetDouble() const = 0;
    virtual const LLBC_String &GetStr() const = 0;

    virtual void SetInt(sint64 iVal) = 0;
    virtual void SetDouble(double val) = 0;
    virtual void SetStr(const char *val) = 0;
    virtual void SetBlob(const char *val, uint32 len) = 0;

protected:
    BaseField(const BaseField &) = delete;
    BaseField &operator=(const BaseField &) = delete;

protected:
    const DBFieldInfo &_info;
    bool _changed = false;
};

