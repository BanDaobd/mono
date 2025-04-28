#pragma once

template<size_t... N>
class ArgsCaptureIndex{};

/*
* 辅助模板实现
*/

template<class Seq, size_t NextI>
struct Append_ArgsCaptureIndex;

template<size_t... Is, size_t NextI>
struct Append_ArgsCaptureIndex<ArgsCaptureIndex<Is...>, NextI> {
    using type = ArgsCaptureIndex<Is..., NextI>;
};

/*
* 递归序列计算，主模板中声明size_t N，而终止条件将N特化为0，那么递归次数为 N->N-1->......->0,为N + 1次递归
* 然后看递归中调用typename Append_ArgsCaptureIndex<typename make_args_capture_index<N - 1>::type, N - 1>::type;的次数，因为这个调用实际插入了数值到size_t list中
* N->N-1(调用)
* N-1->N-2(调用)
* ......
* 1->0(调用)
* 0(无调用返回)
* 调用了N次，且每次调用时插入了箭头后面那个数值
* 而在终止条件中，给出了一个空序列<>，所以从空序列往前累加
* 生成的序列为: 0,1,......,N-1
*/
template<size_t N>
struct make_args_capture_index {
    using type = typename Append_ArgsCaptureIndex<typename make_args_capture_index<N - 1>::type, N - 1>::type;
};

template<>
struct make_args_capture_index<0> {
    using type = ArgsCaptureIndex<>;
};
