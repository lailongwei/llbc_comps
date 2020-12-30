#include "Record.h"
#include "BaseField.h"
#include <assert.h>

Record::Record(uint32_t fieldNum, std::string tableName, MODE mode)
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
int64_t Record::GetInt(uint32_t idx) const
{
    return _fields[idx]->GetInt();
}

double Record::GetDouble(uint32_t idx) const
{
    return _fields[idx]->GetDouble();
}

const std::string &Record::GetStr(uint32_t idx) const
{
    return _fields[idx]->GetStr();
}

/**
 * 通过数据域名称获取值
 */
int64_t Record::GetInt(const char *name) const
{
    return GetInt(GetFieldIdx(name));
}

double Record::GetDouble(const char *name) const
{
    return GetDouble(GetFieldIdx(name));
}

const std::string &Record::GetStr(const char *name) const
{
    return GetStr(GetFieldIdx(name));
}

uint32_t Record::GetFieldIdx(const char *name) const
{
    auto it = _fieldName2Idx.find(name);
    assert(it != _fieldName2Idx.end());
    return it->second;
}

uint32_t Record::GetFieldNum() const
{
    return _fieldNum;
}

void Record::WriteField(uint32_t idx, BaseField *field)
{
    _fieldName2Idx[std::string(field->GetName())] = idx;
    _fields[idx].swap(std::unique_ptr<BaseField>(field));
}

void Record::ClsEditFlag(void)
{
    for (auto &field : _fields)
    {
        field->TagChanged(false);
    }
}

bool Record::IsChanged(uint32_t idx) const
{
    return _fields[idx]->IsChanged();
}

int32_t Record::GetKeyIdx() const
{
    return _keyIdx;
}

void Record::SetKeyIdx(int32_t idx)
{
    _keyIdx = idx;
}

int32_t Record::GetAutoIncIdx() const
{
    return _autoIncIdx;
}

void Record::SetAutoIncIdx(int32_t idx)
{
    _autoIncIdx = idx;
}

const std::string &Record::GetTableName() const
{
    return _tableName;
}

MODE Record::GetMode() const
{
    return _mode;
}

void Record::SetInt(uint32_t idx, int64_t val)
{
    _fields[idx]->SetInt(val);
}

void Record::SetDouble(uint32_t idx, double val)
{
    _fields[idx]->SetDouble(val);
}

void Record::SetStr(uint32_t idx, const char *val)
{
    _fields[idx]->SetStr(val);
}

void Record::SetBlob(uint32_t idx, const char *val, uint32_t len)
{
    _fields[idx]->SetBlob(val, len);
}

void Record::SetInt(const char *name, int64_t val)
{
    SetInt(GetFieldIdx(name), val);
}

void Record::SetDouble(const char *name, double val)
{
    SetDouble(GetFieldIdx(name), val);
}

void Record::SetStr(const char *name, const char *val)
{
    SetStr(GetFieldIdx(name), val);
}

void Record::SetBlob(const char *name, const char *val, uint32_t len)
{
    SetBlob(GetFieldIdx(name), val, len);
}

const BaseField *Record::GetField(uint32_t idx) const
{
    return _fields[idx].get();
}
