#pragma once


/*
* 取参数包中的第N个参数
*/
template<size_t N, class ...Args>
struct ArgsElement;

template<class T,class ...Args>
struct ArgsElement<0, T, Args...>
{
    using type = T;
};

template<size_t N, class T, class ...Args>
struct ArgsElement<N, T, Args...>
{
    static_assert(N > 0, "out of range");
    using type = typename ArgsElement<N - 1, Args...>::type;
};


/*
* 取参数包中的前N个参数
*/
template<class ...Args>
struct Filledlist
{
	template<class T>
	using push_fornt = Filledlist<T, Args...>;
};


template<size_t N, class ...Args>
struct ArgAdd;


template<class ...Args>
struct ArgAdd<0, Args...>
{
	using list = Filledlist<>;
};

template<size_t N, class T, class ...Args>
struct ArgAdd<N, T, Args...>
{
	using list = typename ArgAdd<N - 1, Args...>::list::template push_fornt<T>;
};