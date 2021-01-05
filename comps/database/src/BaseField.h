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

#include "comp_com/Common.h"
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

