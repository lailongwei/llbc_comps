#include "Recordset.h"

Recordset::Recordset(uint32_t recordNum)
    : _recordNum(recordNum)
{
    _records.resize(recordNum);
}

//从集合充获取一条记录(集合仍然管理Record对象内存)
IRecord *Recordset::GetRecord(uint32_t idx)
{
    return _records[idx].get();
}

//从集合从获取一条记录(集合不再管理Record对象内存)
IRecord *Recordset::PopRecord(uint32_t idx)
{
    return _records[idx].release();
}

//获得集合记录数量
uint32_t Recordset::CountRecord() const
{
    return _recordNum;
}

IRecord &Recordset::operator[](uint32_t idx)
{
    return *(_records[idx]);
}

void Recordset::WriteRecord(uint32_t idx, IRecord *record)
{
    _records[idx].swap(std::unique_ptr<IRecord>(record));
}