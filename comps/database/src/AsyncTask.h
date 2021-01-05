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

enum class AsyncTaskType
{
    None,
    Query,       //查询多条记录
    ExecuteSql,  //执行sql
};

/**
 * 异步任务封装
 */
class AsyncTask
{
public:
    AsyncTask()
        : taskType(AsyncTaskType::None)
    {}
    virtual ~AsyncTask() = default;

    virtual void Invoke() = 0;

public:
    LLBC_String sql;
    AsyncTaskType taskType;
};

/**
 * 查询多条记录
 */
struct __TaskQueryRecordset
{
    MODE mode;
    IRecordset *result = nullptr;
    AsyncQueryCB cb;
};

/**
 * 执行sql
 */
struct __TaskQuerySql
{
    bool result = false;
    AsyncSqlCB cb;
};

/**
* 异步任务模版
*/
template <typename TaskInfo>
class AcyncTaskTemplate : public AsyncTask
{
public:
    virtual void Invoke() override
    {
        std::invoke(task.cb, task.result);
    }

public:
    TaskInfo task;
};

using TaskQuery = AcyncTaskTemplate<__TaskQueryRecordset>;
using TaskQuerySql = AcyncTaskTemplate<__TaskQuerySql>;

