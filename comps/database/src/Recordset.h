#pragma once

#include "IDB.h"

#include <vector>
#include <memory>

class Record;

class Recordset : public IRecordset
{
public:
    Recordset(uint32 recordNum);
    virtual ~Recordset() = default;

public:
    //从集合充获取一条记录(集合仍然管理Record对象内存)
    virtual IRecord *GetRecord(uint32 idx) override;

    //从集合从获取一条记录(集合不再管理Record对象内存)
    virtual IRecord *PopRecord(uint32 idx) override;

    //获得集合记录数量
    virtual uint32 GetSize() const override;

    virtual IRecord &operator[](uint32 idx) override;

public:
    void WriteRecord(uint32 idx, IRecord *record);

private:
    uint32 _recordNum;
    std::vector<std::unique_ptr<IRecord>> _records;
};

