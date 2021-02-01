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

sint32 AsyncTaskQueue::Size()
{
    return _count;
}
