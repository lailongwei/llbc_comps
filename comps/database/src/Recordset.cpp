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

#include "Recordset.h"

Recordset::Recordset(uint32 recordNum)
    : _recordNum(recordNum)
{
    _records.resize(recordNum);
}

//从集合充获取一条记录(集合仍然管理Record对象内存)
IRecord *Recordset::GetRecord(uint32 idx)
{
    return _records[idx].get();
}

//从集合从获取一条记录(集合不再管理Record对象内存)
IRecord *Recordset::PopRecord(uint32 idx)
{
    return _records[idx].release();
}

//获得集合记录数量
uint32 Recordset::GetSize() const
{
    return _recordNum;
}

IRecord &Recordset::operator[](uint32 idx)
{
    return *(_records[idx]);
}

void Recordset::WriteRecord(uint32 idx, IRecord *record)
{
    _records[idx].reset(record);
}