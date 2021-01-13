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
#include <memory>

class Record;

class Recordset : public IRecordset
{
public:
    Recordset(uint32 recordNum);
    virtual ~Recordset() = default;

public:
    // Get a record from set.
    virtual IRecord *GetRecord(uint32 idx) override;

    // Pop a record from set.(release record)
    virtual IRecord *PopRecord(uint32 idx) override;

    // Get record count.
    virtual uint32 GetSize() const override;

    // Get a record ref.
    virtual IRecord &operator[](uint32 idx) override;

public:
    void WriteRecord(uint32 idx, IRecord *record);

private:
    uint32 _recordNum;
    std::vector<std::unique_ptr<IRecord>> _records;
};

