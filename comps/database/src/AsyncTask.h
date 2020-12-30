#ifndef _ASYNC_TASK_H_
#define _ASYNC_TASK_H_

#include "IDB.h"

enum class AsyncTaskType
{
    None,
    QueryRecord,     //查询单条记录
    QueryRecordset,  //查询多条记录
    QuerySql,        //执行sql
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
    std::string sql;
    AsyncTaskType taskType;
};

/**
* 查询单条记录
*/
struct __TaskQueryRecord
{
    MODE mode;
    IRecord *result = nullptr;
    DBAsyncRecordCB cb;
};

/**
 * 查询多条记录
 */
struct __TaskQueryRecordset
{
    MODE mode;
    IRecordset *result = nullptr;
    DBAsyncRecordsetCB cb;
};

/**
 * 执行sql
 */
struct __TaskQuerySql
{
    bool result = false;
    DBAsyncSqlCB cb;
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

using TaskQueryRecord = AcyncTaskTemplate<__TaskQueryRecord>;
using TaskQueryRecordset = AcyncTaskTemplate<__TaskQueryRecordset>;
using TaskQuerySql = AcyncTaskTemplate<__TaskQuerySql>;

#endif  // !_ASYNC_TASK_H_
