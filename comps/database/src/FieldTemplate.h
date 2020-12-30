#ifndef _FIELD_TEMPLATE_H_
#define _FIELD_TEMPLATE_H_

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
    virtual bool SetValue(const char *value, uint32_t len) override;

    virtual int64_t GetInt() const override;
    virtual double GetDouble() const override;
    virtual std::string &GetStr() const override;

    virtual void SetInt(int64_t iVal) override;
    virtual void SetDouble(double val) override;
    virtual void SetStr(const char *val) override;
    virtual void SetBlob(const char *val, uint32_t len) override;

private:
    DataType _val = 0;
};

template <>
class FieldTemplate<std::string> : public BaseField
{
public:
    explicit FieldTemplate(const DBFieldInfo &info)
        : BaseField(info)
    {}

public:
    virtual bool SetValue(const char *value, uint32_t len) override;

    virtual int64_t GetInt() const override;
    virtual double GetDouble() const override;
    virtual const std::string &GetStr() const override;

    virtual void SetInt(int64_t val) override;
    virtual void SetDouble(double val) override;
    virtual void SetStr(const char *val) override;
    virtual void SetBlob(const char *val, uint32_t len) override;

private:
    std::string _val;
};

#include "FieldTemplate.inl"

using UInt64Field = FieldTemplate<uint64_t>;
using UIntField = FieldTemplate<uint32_t>;
using USmallIntField = FieldTemplate<uint16_t>;
using UTinyIntField = FieldTemplate<uint8_t>;
using Int64Field = FieldTemplate<int64_t>;
using IntField = FieldTemplate<int32_t>;
using SmallIntField = FieldTemplate<int16_t>;
using DoubleField = FieldTemplate<double>;
using TinyIntField = FieldTemplate<int8_t>;
using StringField = FieldTemplate<std::string>;
using BlobField = FieldTemplate<std::string>;

#endif  // _FIELD_TEMPLATE_H_
