#ifndef ARG_PATH_HPP
#define ARG_PATH_HPP

#include"arg_path.h"

namespace CMD {

template<typename T>
template<typename... Ts>
ArgPath<T>::ArgPath(const size_t& index, const Ts&... args) : type(Type::Index), index(index) {
    next = std::make_shared<ArgPath>(ArgPath(args...));
}

template<typename T>
template<typename... Ts>
ArgPath<T>::ArgPath(const std::string& key, const Ts&... args) : type(Type::Key), key(key) {
    next = std::make_shared<ArgPath>(ArgPath(args...));
}

template<typename T>
size_t ArgPath<T>::get_index() const {
    if(type != Type::Index)
        throw std::invalid_argument("-E- Getting ArgPath index on key or null node!");
    return index;
}

template<typename T>
const std::string& ArgPath<T>::get_key() const {
    if(type != Type::Key) 
        throw std::invalid_argument("-E- Getting ArgPath key on index or null node!");
    return key;
}

template<typename T>
const ArgPath<T>& ArgPath<T>::get_next() const {
    if(!next)
        throw std::invalid_argument("-E- Getting next node of NULL node!");
    return *(this->next);
}

template<typename T>
template<typename A, typename>
void ArgPath<T>::parse_yaml(A& current, T& variable) {
    parse_yaml(*this, current, variable);
}

template<typename T>
template<typename A, typename>
T ArgPath<T>::parse_yaml(A& current) {
    T variable;
    parse_yaml(*this, current, variable);
    return variable;
}

template<typename T>
void ArgPath<T>::parse_yaml(const ArgPath& arg, Sequence& current, T& variable) {
    if(!arg.IsSequence())
        throw std::invalid_argument("-E- ArgPath got a Sequence but is not of type Index!");
    parse_argument(*next, current[arg.get_index()], *next, variable);
}

template<typename T>
void ArgPath<T>::parse_yaml(const ArgPath& arg, Map& current, T& variable) {
    if(!arg.IsMap())
        throw std::invalid_argument("-E- ArgPath got a Map but is not of type Key!");
    parse_argument(*next, current[arg.get_key()], *next, variable);
}

template<typename T>
void ArgPath<T>::parse_yaml(const ArgPath& arg, Scalar& current, T& variable) {
    if(!arg.IsScalar())
        throw std::invalid_argument("-E- ArgPath got a Scalar but is not of type Leaf!");
    current.get<T>(variable);
}

} // CMD

#endif /* ARG_PATH_HPP */
