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
    int32_t GetKeyIdx() const;
    void SetKeyIdx(int32_t idx);
    //自增Id
    int32_t GetAutoIncIdx() const;
    void SetAutoIncIdx(int32_t idx);

    const LLBC_String &GetTableName() const;
    const BaseField *GetField(uint32 idx) const;
    MODE GetMode() const;

private:
    MODE _mode;
    uint32 _fieldNum;
    LLBC_String _tableName;

    std::vector<std::unique_ptr<BaseField>> _fields;
    std::map<LLBC_String, int> _fieldName2Idx;

    int32_t _keyIdx = -1;
    int32_t _autoIncIdx = -1;  //自增Id位置
};

