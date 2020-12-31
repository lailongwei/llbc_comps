#pragma once

#include "BaseField.h"
#include "mysql.h"
#include <assert.h>

template <typename DataType>
class FieldTemplate : public BaseField
{
public:
    explicit FieldTemplate(const DBFieldInfo &info)
        : BaseField(info)
    {}

public:
    virtual bool SetValue(const char *value, uint32 len) override;

    virtual sint64 GetInt() const override;
    virtual double GetDouble() const override;
    virtual LLBC_String &GetStr() const override;

    virtual void SetInt(sint64 iVal) override;
    virtual void SetDouble(double val) override;
    virtual void SetStr(const char *val) override;
    virtual void SetBlob(const char *val, uint32 len) override;

private:
    DataType _val = 0;
};

template <>
class FieldTemplate<LLBC_String> : public BaseField
{
public:
    explicit FieldTemplate(const DBFieldInfo &info)
        : BaseField(info)
    {}

public:
    virtual bool SetValue(const char *value, uint32 len) override;

    virtual sint64 GetInt() const override;
    virtual double GetDouble() const override;
    virtual const LLBC_String &GetStr() const override;

    virtual void SetInt(sint64 val) override;
    virtual void SetDouble(double val) override;
    virtual void SetStr(const char *val) override;
    virtual void SetBlob(const char *val, uint32 len) override;

private:
    LLBC_String _val;
};

#include "FieldTemplate.inl"

using UInt64Field = FieldTemplate<uint64>;
using UIntField = FieldTemplate<uint32>;
using USmallIntField = FieldTemplate<uint16_t>;
using UTinyIntField = FieldTemplate<uint8_t>;
using Int64Field = FieldTemplate<sint64>;
using IntField = FieldTemplate<int32_t>;
using SmallIntField = FieldTemplate<int16_t>;
using DoubleField = FieldTemplate<double>;
using TinyIntField = FieldTemplate<int8_t>;
using StringField = FieldTemplate<LLBC_String>;
using BlobField = FieldTemplate<LLBC_String>;

