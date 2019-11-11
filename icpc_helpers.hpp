#ifndef ICPC_HELPERS_HPP
#define ICPC_HELPERS_HPP 

#include<functional>
#include<utility>

template<size_t N>
struct apply {
    template<typename F, typename TUP, typename... Ts>
    apply(const F& func,const TUP& args, Ts&... rest) {
        auto current = std::get<N-1>(args);
        apply<N-1>(func, args, current, rest...);
    }
};

template<>
struct apply<0> {
    template<typename F, typename TUP, typename... Ts>
    apply(const F& func,const TUP& args, Ts&... rest) {
        func(rest...);
    }
};

template<size_t N>
struct apply_every_other {
    template<typename F, typename TUP, typename... Ts>
    apply_every_other(const F& func,const TUP& args, Ts&... rest) {
        auto current = std::get<N-1>(args);
        apply_every_other<N-2>(func, args, current, rest...);
    }
};

template<>
struct apply_every_other<0> {
    template<typename F, typename TUP, typename... Ts>
    apply_every_other(const F& func,const TUP& args, Ts&... rest) {
        func(rest...);
    }
};

#endif // ICPC_HELPERS_HPP 
