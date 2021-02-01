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

#include "Record.h"
#include "BaseField.h"
#include <assert.h>

Record::Record(uint32 fieldNum, LLBC_String tableName, MODE mode)
    : _fieldNum(fieldNum)
    , _tableName(std::move(tableName))
    , _mode(mode)
{
    _fields.resize(fieldNum);
}

Record::~Record() {}

/**
 * 通过数据位置索引获取值
 */
sint64 Record::GetInt(uint32 idx) const
{
    return _fields[idx]->GetInt();
}

double Record::GetDouble(uint32 idx) const
{
    return _fields[idx]->GetDouble();
}

const LLBC_String &Record::GetStr(uint32 idx) const
{
    return _fields[idx]->GetStr();
}

/**
 * 通过数据域名称获取值
 */
sint64 Record::GetInt(const LLBC_String& name) const
{
    return GetInt(GetFieldIdx(name));
}

double Record::GetDouble(const LLBC_String& name) const
{
    return GetDouble(GetFieldIdx(name));
}

const LLBC_String &Record::GetStr(const LLBC_String& name) const
{
    return GetStr(GetFieldIdx(name));
}

uint32 Record::GetFieldIdx(const LLBC_String& name) const
{
    auto it = _fieldName2Idx.find(name);
    assert(it != _fieldName2Idx.end());
    return it->second;
}

uint32 Record::GetFieldNum() const
{
    return _fieldNum;
}

void Record::WriteField(uint32 idx, BaseField *field)
{
    _fieldName2Idx[LLBC_String(field->GetName())] = idx;
    _fields[idx].reset(field);
}

void Record::ClsEditFlag(void)
{
    for (auto &field : _fields)
    {
        field->TagChanged(false);
    }
}

bool Record::IsChanged(uint32 idx) const
{
    return _fields[idx]->IsChanged();
}

sint32 Record::GetKeyIdx() const
{
    return _keyIdx;
}

void Record::SetKeyIdx(sint32 idx)
{
    _keyIdx = idx;
}

sint32 Record::GetAutoIncIdx() const
{
    return _autoIncIdx;
}

void Record::SetAutoIncIdx(sint32 idx)
{
    _autoIncIdx = idx;
}

const LLBC_String &Record::GetTableName() const
{
    return _tableName;
}

MODE Record::GetMode() const
{
    return _mode;
}

void Record::SetInt(uint32 idx, sint64 val)
{
    _fields[idx]->SetInt(val);
}

void Record::SetDouble(uint32 idx, double val)
{
    _fields[idx]->SetDouble(val);
}

void Record::SetStr(uint32 idx, const char *val)
{
    _fields[idx]->SetStr(val);
}

void Record::SetBlob(uint32 idx, const char *val, uint32 len)
{
    _fields[idx]->SetBlob(val, len);
}

void Record::SetInt(const LLBC_String& name, sint64 val)
{
    SetInt(GetFieldIdx(name), val);
}

void Record::SetDouble(const LLBC_String& name, double val)
{
    SetDouble(GetFieldIdx(name), val);
}

void Record::SetStr(const LLBC_String& name, const char *val)
{
    SetStr(GetFieldIdx(name), val);
}

void Record::SetBlob(const LLBC_String& name, const char *val, uint32 len)
{
    SetBlob(GetFieldIdx(name), val, len);
}

const BaseField *Record::GetField(uint32 idx) const
{
    return _fields[idx].get();
}
