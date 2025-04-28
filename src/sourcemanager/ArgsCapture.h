#pragma once

#include<type_traits>
#include"ArgsCaptureIndex.h"
#include"tools.h"

/*
* 用于在lambda表达式的初始化器表达式中展开参数包并保存形参的值
* 需要保存参数类型以及其对应的值
*/


/*
ArgsCaptureIndex用于提供索引并且防止参数包中的相同类型问题，由于需要保存参数的值，所以不能应用EBO
暂时不考虑submit中，工作函数的参数与参数包中对应的值兼容但类型不同时的隐式转换问题
*/
template<size_t N, class T>
struct Base
{
public:
    template<class U>
    Base(U&& value) :t(std::forward<U>(value)) {}
public:
    T t;
};


template<class S, class...Args>
struct ArgsCapture;



template<size_t ...Index, class ...Args>
struct ArgsCapture<ArgsCaptureIndex<Index...>, Args...> :
    public Base<Index, typename ArgsElement<Index, Args...>::type>...
{
public:
    template<class ...CArgs>
    ArgsCapture(CArgs&& ...args) :
        Base<Index, typename ArgsElement<Index, Args...>::type>(std::forward<CArgs>(args))...
    {

        /*
        关于...运算符
        展开方式如下：
        外层...会找到其临近最大Pattern中所有的参数包名称(包括类型参数包和值参数包)，然后同步的对其进行展开。
        而对于Args...这种参数包展开模式(Args是参数包名而Args...是展开模式)，不会做任何操作

        上面的构造器会先展开为
        Base<0, typename ArgsElement<0,Args...>::type>(std::forward<T0>(args[0])),
        Base<1, typename ArgsElement<1,Args...>::type>(std::forward<T1>(args[1])),
        Base<2, typename ArgsElement<2,Args...>::type>(std::forward<T2>(args[2])),
        ......
        Base<n, typename ArgsElement<n,Args...>::type>(std::forward<Tn>(args[n]))。
        每个被展开的模式再对表达式typename ArgsElement<n,Args...>::type进行求值，
        至少能确定在外层...展开结束前(展开到最后一个n时)，前n-1个展开式中的typename ArgsElement<2,Args...>::type已经求值结束，但是求值顺序不能保证
        */

        /*
        这里展开后的基类构造函数会自然的和其所属的基类匹配，上面的继承会在构造函数调用之前先展开完毕，展开方式与上面所说的相同
        因此只要...args中的值和...Args的类型在顺序上一一对应，就能正确的向对应的基类构造函数传递参数

        继承中：
        public
        Base<0,T1>,
        Base<1,T2>,
        Base<2,T3>
        ...

        构造函数中：
        Base<0,T1>(args[0]),
        Base<1,T2>(args[1]),
        Base<2,T3>(args[2]),
        ......

        基类Base<0,T1>,对应的构造函数是Base<0,T1>(args[0]),
        因此只要args[n]对应正确的类型Tn,那么构造函数就会正确的传递参数
        */
    }
};

template<class ...Args>
struct Args_tuple :
    public ArgsCapture<typename make_args_capture_index<sizeof...(Args)>::type, Args...>
{
public:
    Args_tuple(Args&& ...args) :ArgsCapture<typename make_args_capture_index<sizeof...(Args)>::type, Args...>(std::forward<Args>(args)...) {}
};


/*
* get函数实现
* 考虑const和decay
*/
template<size_t N, class ...Args>
decltype(auto) getV(Args_tuple<Args...>& obj)
{
    static_assert(N < sizeof...(Args), "out of range");
    /*
    取子类地址将其转换为对应索引的父类并返回父类的成员变量
    */
    return static_cast<Base<N, typename ArgsElement<N, Args...>::type>*>(&obj)->t;
}

/*
* apply函数实现
*/

template<class F, class Tuple, size_t ...Index>
decltype(auto) applyFImpl(F&& f, Tuple&& t, ArgsCaptureIndex<Index...>)
{
    /*
    这里的展开模式和上面相同，找到参数包名Index
    展开：
    std::invoke(f,
    getV<0>(std::forward<Tuple>(t)),
    getV<1>(std::forward<Tuple>(t)),
    getV<2>(std::forward<Tuple>(t)),
    ......
    )
    */
    return std::invoke(f, getV<Index>(std::forward<Tuple>(t))...);
}


template<class F, class ...Args>
decltype(auto) applyF(F&& f, Args_tuple<Args...>&& obj)
{
    return applyFImpl(
        std::forward<F>(f),
        std::forward<Args_tuple<Args...>>(obj),
        typename make_args_capture_index<sizeof...(Args) - 1>::type{}
    );
}