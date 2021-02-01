// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "BaseField.h"
#include <mysql/mysql.h>
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

/**
 * string template specializations.
 */
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
using USmallIntField = FieldTemplate<uint16>;
using UTinyIntField = FieldTemplate<uint8>;
using Int64Field = FieldTemplate<sint64>;
using IntField = FieldTemplate<sint32>;
using SmallIntField = FieldTemplate<sint16>;
using DoubleField = FieldTemplate<double>;
using TinyIntField = FieldTemplate<sint8>;
using StringField = FieldTemplate<LLBC_String>;
using BlobField = FieldTemplate<LLBC_String>;

