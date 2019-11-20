#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP

#include"argument.h"
#include<sstream>

namespace CMD {
/* Helper declerations */
template<typename... Ts>
void fill_sequence(std::vector<std::shared_ptr<Argument>>&, Ts...);

template<typename... Ts>
void fill_map(std::unordered_map<std::string, std::shared_ptr<Argument>>&, Ts...);

/* Constructors */
template<typename... Ts>
Sequence::Sequence(Dependencies dependencies, Ts... args)
                  : Argument(dependencies) {
    fill_sequence(this->args, std::forward<Ts>(args)...);
}

template<typename... Ts>
Map::Map(Dependencies dependencies, Ts... args)
         : Argument(dependencies) {
    fill_map(this->args, std::forward<Ts>(args)...);
}

/* clones */
std::shared_ptr<Argument> clone_argument(std::shared_ptr<Argument> arg) {
    std::shared_ptr<Map> map;
    std::shared_ptr<Sequence> seq;
    std::shared_ptr<Scalar> scalar;
    if((map = std::dynamic_pointer_cast<Map>(arg)))
        return std::make_shared<Map>(map->clone());
    if((seq = std::dynamic_pointer_cast<Sequence>(arg)))
        return std::make_shared<Sequence>(seq->clone());
    if((scalar = std::dynamic_pointer_cast<Scalar>(arg)))
        return std::make_shared<Scalar>(scalar->clone());
    throw std::invalid_argument("-E- Argument was not one of Map, Sequence, or Scalar!");
}

Map Map::clone() const {
    Map temp;
    temp.dependencies = dependencies;
    for(auto& [key, value] : args)
        temp.args[key] = clone_argument(value);
    return temp;
}

Sequence Sequence::clone() const {
    Sequence temp;
    temp.dependencies = dependencies;
    for(auto& value : args)
        temp.args.push_back(clone_argument(value));
    return temp;
}

Scalar Scalar::clone() const {
    Scalar temp{dependencies};
    temp.val = val;
    temp.repr = repr;
    return temp;
}

/* Getting and setting */
Argument& Sequence::operator[](size_t index) {
    return *args[index];
}

Argument& Map::operator[](const std::string& name) {
    auto result = args.find(name);
    if(result == args.end())
        throw std::invalid_argument("-E- Got key '" + name + "' which does not exist in Map!");
    return *(result->second);
}

void Scalar::set_repr(const std::string& repr) {
    this->repr = repr;
    this->val.reset();
}
template<typename T>
void Scalar::set(const std::string& repr) {
    std::istringstream ss(repr);
    T value;
    if(!(ss>>value) || std::istream::sentry(ss)) {
        throw std::invalid_argument("-E- Failed to convert '" + repr + "' to approximate type " +
                                    typeid(T).name() + "!");
    }
    this->val = value;
}

template<typename T>
T Scalar::get(T& var) {
    try {
        var = std::any_cast<T>(val);
    } catch (const std::bad_any_cast& e) {
        if(repr.empty()) {
            if(!val.has_value())
                throw std::invalid_argument("-E- Attempeting to get unset argument with variable of approximate type " +
                                            std::string(typeid(T).name()) +"!");
            throw std::invalid_argument("-E- Trying to get argument of approximate type " +
                                        std::string(val.type().name()) +
                                        " with variable of approximate type " + typeid(T).name()
                                        + "!");
        }
        set<T>(repr);
        repr.clear();
        get(var);
    }
    return var;
}

/* Printing */
std::ostream& Sequence::print(std::ostream& os, const std::string& spaces) const {
    for(auto& p_arg : args) {
        os<<spaces<<"-\n";
        p_arg->print(os, spaces + "  ");
    }
    return os;
}

std::ostream& Map::print(std::ostream& os, const std::string& spaces) const {
    for(auto const& [key, p_arg]: args) {
        os<<spaces<<key<<":\n";
        p_arg->print(os, spaces + std::string(key.size(), ' '));
    }
    return os;
}

std::ostream& Scalar::print(std::ostream& os, const std::string& spaces) const {
    os<<spaces;
    if(!val.has_value()) os<<"null";
    else os<<val.type().name();
    os<<"\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Argument& arg) {
    arg.print(os);
    return os;
}

/* constructor template helpers */
template<typename T, typename... Ts>
void fill_sequence(std::vector<std::shared_ptr<Argument>>& args, T& current, Ts... rest) {
    args.push_back(std::shared_ptr<T>(&current, [](const T*){}));
    fill_sequence(args, std::forward<Ts>(rest)...);
}

template<typename T, typename... Ts>
void fill_sequence(std::vector<std::shared_ptr<Argument>>& args, T current, Ts... rest) {
    args.emplace_back(std::make_shared<T>(current));
    fill_sequence(args, std::forward<Ts>(rest)...);
}

template<typename T>
void fill_sequence(std::vector<std::shared_ptr<Argument>>& args, T& current) {
    args.push_back(std::shared_ptr<T>(&current(&current, [](const T*){})));
}

template<typename T>
void fill_sequence(std::vector<std::shared_ptr<Argument>>& args, T current) {
    args.emplace_back(std::make_shared<T>(current));
}

template<typename S, typename T, typename... Ts>
void fill_map(std::unordered_map<std::string, std::shared_ptr<Argument>>& args,
              const S& key, T& current, Ts... rest) {
    args[key] = std::shared_ptr<T>(&current, [](const T*){});
    fill_map(args, std::forward<Ts>(rest)...);
}

template<typename S, typename T, typename... Ts>
void fill_map(std::unordered_map<std::string, std::shared_ptr<Argument>>& args,
              const S& key, T current, Ts... rest) {
    args[key] = std::make_shared<T>(current);
    fill_map(args, std::forward<Ts>(rest)...);
}

void fill_map(std::unordered_map<std::string, std::shared_ptr<Argument>>& args) {}

} // CMD

#endif /* ARGUMENT_HPP */
