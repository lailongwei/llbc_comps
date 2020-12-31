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

