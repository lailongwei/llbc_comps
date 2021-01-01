#include "AsyncTaskQueue.h"

void AsyncTaskQueue::Push(AsyncTask *task)
{
    _lock.lock();
    _tasks.push(task);
    _lock.unlock();

    _cv.notify_one();
}

AsyncTask *AsyncTaskQueue::TimePop(int interval)
{
    while (true)
    {
        std::unique_lock<std::mutex> l(_lock);
        _cv.wait_for(l, std::chrono::milliseconds(interval), [this]() { return !_tasks.empty(); });

        if (_tasks.empty())
            return nullptr;

        auto task = _tasks.front();
        _tasks.pop();
        return task;
    }
}

AsyncTask *AsyncTaskQueue::TryPop()
{
    _lock.lock();
    if (_tasks.empty())
    {
        _lock.unlock();
        return nullptr;
    }

    auto task = _tasks.front();
    _tasks.pop();
    _lock.unlock();

    return task;
}

void AsyncTaskQueue::PopAll(std::vector<AsyncTask *> &tasks)
{
    _lock.lock();
    tasks.reserve(_tasks.size());

    while (!_tasks.empty())
    {
        tasks.push_back(_tasks.front());
        _tasks.pop();
    }
    _lock.unlock();
}

void AsyncTaskQueue::TimePopAll(std::vector<AsyncTask *> &tasks, int interval)
{
    while (true)
    {
        std::unique_lock<std::mutex> l(_lock);
        _cv.wait_for(l, std::chrono::milliseconds(interval), [this]() { return !_tasks.empty(); });

        if (_tasks.empty())
            return ;

        tasks.reserve(_tasks.size());
        while (!_tasks.empty())
        {
            tasks.push_back(_tasks.front());
            _tasks.pop();
        }
        return;
    }
}

bool AsyncTaskQueue::Empty()
{
    return _count == 0;
}

int32_t AsyncTaskQueue::Size()
{
    return _count;
}
