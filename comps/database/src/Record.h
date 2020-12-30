#ifndef _RECORD_H_
#define _RECORD_H_

#include "IDB.h"
#include <vector>
#include <map>
#include <memory>

class BaseField;

class Record : public IRecord
{
public:
    Record(uint32_t fieldNum, std::string tableName, MODE mode);
    virtual ~Record();

public:
    /**
     * 通过数据位置索引获取值
     */
    virtual int64_t GetInt(uint32_t idx) const override;
    virtual double GetDouble(uint32_t idx) const override;
    virtual const std::string &GetStr(uint32_t idx) const override;
    /**
     * 通过数据域名称获取值
     */
    virtual int64_t GetInt(const char *name) const override;
    virtual double GetDouble(const char *name) const override;
    virtual const std::string &GetStr(const char *name) const override;

public:
    void WriteField(uint32_t idx, BaseField *field);

    uint32_t GetFieldIdx(const char *name) const;
    uint32_t GetFieldNum() const;

public:
    void ClsEditFlag(void);
    bool IsChanged(uint32_t idx) const;
    //主键
    int32_t GetKeyIdx() const;
    void SetKeyIdx(int32_t idx);
    //自增Id
    int32_t GetAutoIncIdx() const;
    void SetAutoIncIdx(int32_t idx);

    const std::string &GetTableName() const;
    MODE GetMode() const;

    virtual void SetInt(uint32_t idx, int64_t val) override;
    virtual void SetDouble(uint32_t idx, double val) override;
    virtual void SetStr(uint32_t idx, const char *val) override;
    virtual void SetBlob(uint32_t idx, const char *val, uint32_t len) override;

    virtual void SetInt(const char *name, int64_t val) override;
    virtual void SetDouble(const char *name, double val) override;
    virtual void SetStr(const char *name, const char *val) override;
    virtual void SetBlob(const char *name, const char *val, uint32_t len) override;

    const BaseField *GetField(uint32_t idx) const;

private:
    MODE _mode;
    uint32_t _fieldNum;
    std::string _tableName;

    std::vector<std::unique_ptr<BaseField>> _fields;
    std::map<std::string, int> _fieldName2Idx;

    int32_t _keyIdx = -1;
    int32_t _autoIncIdx = -1;  //自增Id位置
};

#endif  // _RECORD_H_
