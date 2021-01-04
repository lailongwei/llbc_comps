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

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <vector>

class AsyncTask;

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
    std::atomic_int _count = ATOMIC_FLAG_INIT;

    std::queue<AsyncTask *> _tasks;
};

