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

#include "IDB.h"
#include <vector>
#include <map>
#include <memory>

class BaseField;

class Record : public IRecord
{
public:
    Record(uint32 fieldNum, LLBC_String tableName, MODE mode);
    virtual ~Record();

public:
    virtual sint64 GetInt(uint32 idx) const override;
    virtual double GetDouble(uint32 idx) const override;
    virtual const LLBC_String &GetStr(uint32 idx) const override;

    virtual sint64 GetInt(const LLBC_String& name) const override;
    virtual double GetDouble(const LLBC_String& name) const override;
    virtual const LLBC_String &GetStr(const LLBC_String& name) const override;

public:
    virtual void SetInt(uint32 idx, sint64 val) override;
    virtual void SetDouble(uint32 idx, double val) override;
    virtual void SetStr(uint32 idx, const char *val) override;
    virtual void SetBlob(uint32 idx, const char *val, uint32 len) override;

    virtual void SetInt(const LLBC_String& name, sint64 val) override;
    virtual void SetDouble(const LLBC_String& name, double val) override;
    virtual void SetStr(const LLBC_String& name, const char *val) override;
    virtual void SetBlob(const LLBC_String& name, const char *val, uint32 len) override;

public:
    void WriteField(uint32 idx, BaseField *field);

    uint32 GetFieldIdx(const LLBC_String& name) const;
    uint32 GetFieldNum() const;

public:
    void ClsEditFlag(void);
    bool IsChanged(uint32 idx) const;
    //主键
    sint32 GetKeyIdx() const;
    void SetKeyIdx(sint32 idx);
    //自增Id
    sint32 GetAutoIncIdx() const;
    void SetAutoIncIdx(sint32 idx);

    const LLBC_String &GetTableName() const;
    const BaseField *GetField(uint32 idx) const;
    MODE GetMode() const;

private:
    MODE _mode;
    uint32 _fieldNum;
    LLBC_String _tableName;

    std::vector<std::unique_ptr<BaseField>> _fields;
    std::map<LLBC_String, int> _fieldName2Idx;

    sint32 _keyIdx = -1;
    sint32 _autoIncIdx = -1;  //自增Id位置
};

