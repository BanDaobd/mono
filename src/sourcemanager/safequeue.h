#include<thread>
#include<atomic>
#include<functional>
#include<mutex>

class Safequeue
{
public:
    Safequeue() = default;
    ~Safequeue() = default;
    

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool stop_flag = false;
    std::atomic_bool empty_flag = true;
    std::function<void()> func;
    std::thread worker_thread;
};