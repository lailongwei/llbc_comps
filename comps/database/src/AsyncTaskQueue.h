#pragma once

#include "AsyncTask.h"
#include <mutex>
#include <queue>
#include <vector>

class AsyncTaskQueue
{
public:
    AsyncTaskQueue() = default;
    ~AsyncTaskQueue() = default;

public:
    void Push(AsyncTask *task);

    AsyncTask *TimePop(int interval);
    AsyncTask *TryPop();

    void PopAll(std::vector<AsyncTask *> &tasks);
    void TimePopAll(std::vector<AsyncTask *> &tasks, int interval);

    bool Empty();
    int32_t Size();

private:
    std::mutex _lock;
    std::condition_variable _cv;
    std::atomic_int _count = 0;

    std::queue<AsyncTask *> _tasks;
};

