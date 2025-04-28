#pragma once
#include<mutex>
#include<condition_variable>
#include<future>
#include"ArgsCapture.h"

//将promise打包到任务函数中，然后将所有权转移到工作线程中防止外部获取future时出现promise_broken
template<class R,class ...Args>
class MyPackaged_task
{
public:
	template<class F, class ...CArgs>
	MyPackaged_task(F&& f, CArgs&& ...args) :
		func(std::forward<F>(f)), tuple(std::forward<CArgs>(args)...), promise(),execute_flag(false)
	{
	}

	MyPackaged_task(MyPackaged_task<R,Args...>&& obj)noexcept :
		func(std::move(obj.func)),execute_flag(std::move(obj.execute_flag)),promise(std::move(obj.promise)),tuple(std::move(obj.tuple))
	{
	}

	MyPackaged_task<R,Args...>&& operator=(MyPackaged_task<R,Args...>&& obj)
	{
		if (&obj == *this)
			return *this;
		this->func = std::move(obj.func);
		this->execute_flag = std::move(obj.execute_flag);
		this->promise = std::move(obj.promise);
		this->tuple = obj.tuple;
	}

	MyPackaged_task(const MyPackaged_task<R, Args...>& obj) = delete;
	MyPackaged_task& operator=(const MyPackaged_task<R, Args...>& obj) = delete;

	void operator()()
	{
		//如果已经执行过，不需要再次执行
		if (execute_flag)
			return;

		try
		{
			if constexpr (std::is_void_v<R>)
			{
				applyF(func, tuple);
				promise.set_value();
			}
			else
			{
				promise.set_value(applyF(func, tuple));
			}

			execute_flag = true;
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	}

	std::future<R> get_future()
	{
		return promise.get_future();
	}

private:
	std::function<R(Args...)> func;
	Args_tuple<Args...> tuple;
	std::promise<R> promise;
	bool execute_flag = false;
};