#pragma once
#include<vector>
#include<thread>
#include<atomic>
#include<functional>
#include<mutex>
#include<condition_variable>
#include<future>
#include"safequeue.h"


class ThreadPool
{
public:
    ThreadPool(size_t _numThreads);
    ~ThreadPool();

    template<class F, class ...Args>
    auto submit(F&& func,Args&& ...args)->std::future<std::invoke_result_t<F,Args...>>;

private:
    void worker_main_loop();

private:

    size_t numThreads;

    std::vector<std::thread> thread_works;

    Safequeue queue;

    std::atomic_bool stop_flag = false;

};