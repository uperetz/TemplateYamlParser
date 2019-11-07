#ifndef ARGUMENT_H
#define ARGUMENT_H

#include<string>
#include<any>
#include<stdexcept>

class Argument {
    public:
    
    class RequiredException;

    Argument() {}
    Argument(std::any default_value) : default_value(default_value) {}

    void set(const std::string& representation) { repr = representation; }

    template<typename T>
    T put(T& lvalue, const std::string& name = "", int index = -1);

    private:

    template<typename T>
    void cast_repr(T&);

    std::string repr;
    std::any default_value;
};

#include"argument.hpp"

#endif // ARGUMENT_H
