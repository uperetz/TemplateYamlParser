#ifndef YAML_HPP
#define YAML_HPP 

#include"argument.h"
#include<sstream>

namespace YAML {

template<typename... Ts>
void fill_sequence(std::vector<std::unique_ptr<Argument>>, Ts...);

template<typename... Ts>
Sequence::Sequence(bool required, Dependencies& dependencies, const Ts&... args) 
                  : Argument(required, dependencies) {
    fill_sequence(this->args, args...);   
}

std::ostream& Sequence::print(std::ostream& os) {
    for(auto&& p_arg : args) {
        p_arg->print(os);
        os<<" ";
    }
    return os;
}

template<typename T>
std::ostream& Scalar<T>::print(std::ostream& os) {
    if(isnull())
        os<<"null";
    else os<<val;
    return os;
}

std::ostream& operator<<(std::ostream& os, Argument& arg) {
    arg.print(os);
    return os;
}

template<typename T>
Scalar<T>& Scalar<T>::operator=(const Scalar<T>& other) {
    val = other.val; // Does not matter if is null;
    null = other.null;
    return *this;
}

template<typename T>
T Scalar<T>::get() {
    if(isnull())
        throw std::runtime_error("-E- Attempting to get a scalar that was not assigned!");
    return val;
}

template<typename T>
void Scalar<T>::set(const std::string& value) {
    std::stringstream ss(value);
    ss>>val;
    null = false;
}

template<typename T, typename... Ts>
void fill_sequence(std::vector<std::unique_ptr<Argument>>& args, const T& current,
                   const Ts&... rest) {
    args.push_back(std::make_unique<T>(current));
    fill_sequence(args, rest...);
}


template<typename T>
void fill_sequence(std::vector<std::unique_ptr<Argument>>& args, const T& current) {
    args.push_back(std::make_unique<T>(current));
}

} // YAML

#endif /* YAML_HPP */
