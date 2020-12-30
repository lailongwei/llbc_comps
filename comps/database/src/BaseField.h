#ifndef _BaseField_H_
#define _BaseField_H_

#include <string>

struct DBFieldInfo
{
    std::string name;
    uint32_t type = 0;
    uint32_t attr = 0;
    uint32_t len = 0;
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
    inline uint32_t GetType(void) const
    {
        return _info.type;
    }
    inline uint32_t GetAttr(void) const
    {
        return _info.attr;
    }
    inline size_t GetLen(void) const
    {
        return _info.len;
    }

public:
    virtual bool SetValue(const char *value, uint32_t len) = 0;

    virtual int64_t GetInt() const = 0;
    virtual double GetDouble() const = 0;
    virtual const std::string &GetStr() const = 0;

    virtual void SetInt(int64_t iVal) = 0;
    virtual void SetDouble(double val) = 0;
    virtual void SetStr(const char *val) = 0;
    virtual void SetBlob(const char *val, uint32_t len) = 0;

protected:
    BaseField(const BaseField &) = delete;
    BaseField &operator=(const BaseField &) = delete;

protected:
    const DBFieldInfo &_info;
    bool _changed = false;
};

#endif
