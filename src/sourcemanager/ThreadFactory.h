#include<thread>
#include<string>
#include<atomic>
#include<functional>

class ThreadFactory
{

public:

    ThreadFactory(std::string name):name_prefix(name),thread_count(0){
        
    }

    template<typename F, typename... Args>
    std::thread createThread(F&& f, Args&&... args) {
        std::thread t(std::forward<F>(f), std::forward<Args>(args)...);
    }

    ~ThreadFactory(){

    }


private:
    std::string name_prefix;

    std::atomic<long> thread_count;
};