#ifndef ARG_PATH_HPP
#define ARG_PATH_HPP

#include"arg_path.h"

namespace CMD {

/* helper */
template<typename MapOrSeq,
         typename = typename std::enable_if<std::is_same<MapOrSeq, Map>::value>::type,
         typename... Ts>
ArgPath<Map&> partial_path(const Ts&... args) {
    return ArgPath<Map&>(args...);
}
template<typename MapOrSeq,
         typename = typename std::enable_if<std::is_same<MapOrSeq, Sequence>::value>::type,
         typename... Ts>
ArgPath<Sequence&> partial_path(const Ts&... args) {
    return ArgPath<Sequence&>(args...);
}

/* Constructors*/
template<>
ArgPath<Sequence&>::ArgPath() : type(Type::Index) {}
template<>
ArgPath<Map&>::ArgPath() : type(Type::Key) {}
template<typename T>
ArgPath<T>::ArgPath() : type(Type::Leaf) {}

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

/* Getters */
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

/* Parse */
template<typename T>
template<typename A, typename>
void ArgPath<T>::parse_yaml(A& current, T& variable) const {
    variable = parse_yaml(*this, current);
}

template<typename T>
template<typename A, typename>
T ArgPath<T>::parse_yaml(A& current) const {
    return parse_yaml(*this, current);
}

template<typename T>
T ArgPath<T>::parse_yaml(const ArgPath& arg, Sequence& current) const {
    if(!arg.IsSequence())
        throw std::invalid_argument("-E- ArgPath got a Sequence but is not of type Index!");
    return parse_argument(*next, current[arg.get_index()]);
}

template<>
Sequence& ArgPath<Sequence&>::parse_yaml(const ArgPath& arg, Sequence& current) const {
    if(!arg.IsSequence())
        throw std::invalid_argument("-E- ArgPath got a Sequence but is not of type Index!");
    if(!next) return current;
    return parse_argument(*next, current[arg.get_index()]);
}

template<typename T>
T ArgPath<T>::parse_yaml(const ArgPath& arg, Map& current) const {
    if(!arg.IsMap())
        throw std::invalid_argument("-E- ArgPath got a Map but is not of type Key!");
    return parse_argument(*next, current[arg.get_key()]);
}

template<>
Map& ArgPath<Map&>::parse_yaml(const ArgPath& arg, Map& current) const {
    if(!arg.IsMap())
        throw std::invalid_argument("-E- ArgPath got a Map but is not of type Key!");
    if(!next) return current;
    return parse_argument(*next, current[arg.get_key()]);
}

template<typename T>
T ArgPath<T>::parse_yaml(const ArgPath& arg, Scalar& current) const {
    if(!arg.IsScalar())
        throw std::invalid_argument("-E- ArgPath got a Scalar but is not of type Leaf!");
    T variable;
    current.get(variable);
    return variable;
}

template<>
Sequence& ArgPath<Sequence&>::parse_yaml(const ArgPath& arg, Scalar& current) const {
    throw std::invalid_argument("-E- ArgPath got a Scalar but is expecting to end on a sequence!");
}
template<>
Map& ArgPath<Map&>::parse_yaml(const ArgPath& arg, Scalar& current) const {
    throw std::invalid_argument("-E- ArgPath got a Scalar but is expecting to end on a Map!");
}

/* Generic Helper for arguments */
template<typename T>
T ArgPath<T>::parse_argument(const ArgPath<T>& next, CMD::Argument& arg) const {
    Sequence* seq = NULL;
    Map* map = NULL;
    Scalar* scalar = NULL;
    if(next.IsSequence()) {
        if(!(seq = dynamic_cast<Sequence*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Sequence!");
        return next.parse_yaml(next, *seq);
    } else if(next.IsMap()) {
        if(!(map = dynamic_cast<Map*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Map!");
        return next.parse_yaml(next, *map);
    } else if(next.IsScalar()) {
        if(!(scalar = dynamic_cast<Scalar*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Scalar!");
        return next.parse_yaml(next, *scalar);
    } else {
        throw std::invalid_argument("-E- Badly formed Yaml! Not one of Map, Sequence, or Scalar.");
    }
}

} // CMD

#endif /* ARG_PATH_HPP */
