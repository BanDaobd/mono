#include"ThreadPool.h"
#include"ArgsCapture.h"
#include"MyPackaged_task.h"

ThreadPool::ThreadPool(size_t _numThreads)
{
    numThreads = numThreads > std::thread::hardware_concurrency() ? numThreads : std::thread::hardware_concurrency();
    if(numThreads == 0) {
        numThreads = 1; // Fallback to at least one thread
    }
    thread_works.reserve(numThreads);

    /*
    在构造函数中捕获this，只用于调用成员函数不访问任何成员变量
    包装主循环
    */
    for(size_t i = 0; i < numThreads; ++i) {
        auto callable = [this](){
            this->worker_main_loop();
        };
    }

    //线程工厂将主循环包装为工作线程

    //保存工作线程

}

//析构
ThreadPool::~ThreadPool()
{

}

//还需要一个任务启动函数和一个任务停止函数
void start()
{

}

void stop()
{

}

void ThreadPool::worker_main_loop()
{
    // Worker thread logic goes here
    // For example, you can use a condition variable to wait for tasks
    while(true)
    {
        std::function<void()> task;
        //从任务队列获取任务，上锁
        {
            std::unique_lock<std::mutex> lock;
        }
        {
            try
            {
                //并行执行取出的任务，不上锁
            }
            catch(...)
            {
                //在这里编写出现异常时的代码，让主循环能够继续正确运行，并且记录日志
            }
            
        }
        // 检测标志以控制循环停止
        if(stop_flag.load(std::memory_order_acquire)) {
            break;
        }
    }
}

/*
工作函数将在主循环中异步执行
需要返回一个存储结果的future对象
一个存储可能的异常的promise对象
*/
template<class F, class ...Args>
auto ThreadPool::submit(F&& func,Args&& ...args)->std::future<std::invoke_result_t<F,Args...>>
{
    using Ret = std::invoke_result_t<F,Args...>;

    //防止Args推导出左值引用导致超出局部变量范围的悬空引用
    using TaskArgs = std::tuple<std::decay_t<Args_>...>; 

    auto packed_task = std::make_unique<MyPackaged_task<Ret, std::decay_t<Args_>...>>(
        std::forward<F>(func),
        std::forward<Args_>(args)...
    );

    std::future<Ret> future = packed_task->get_future();

    //包装为队列可处理的统一类型function<void()>
    auto task_for_queue = [task = std::move(packed_task)]() mutable {
        (*task)();
    };

    //safequeue->enqueue(std::move(task_for_queue));

    return future;

//    lambda表达式的存储和拷贝/移动
//    初始化器捕获Args_tuple实现，将形如T func(A a, B b, ......)转换为std::function<T()>
//    处理func的返回值，包装到future的共享对象中,进一步将std::function<T()>包装为std::function<void()>
//    使用deacy_t去掉引用符号，防止在lambda表达式被调用时使用引用接收到超出作用域的左值局部变量
//    [c_func = std::forward<F>(func), c_args = Args_tuple<std::decay_t<Args...>>(args)... ]() mutable ->decltype(std::future<Ret>)
//     {
//         //在这里捕获工作函数可能发生的任何异常，使用异步工具返回
//         try
//         {
//            applyF(c_func, c_args);
//         }
//         catch(...)
//         {
//             //将异常传递到外部
//         }
//     };

    //future获取返回值

    //包装为function<void()>

    //加入任务队列
}

template<class F, class ...Args>
std::future<std::function<F(Args...)>> submit1(F&& func,Args&& ...args)
{
    
}