#ifndef ARGUMENT_H
#define ARGUMENT_H

#include<string>
#include<stdexcept>
#include<boost/any.hpp>
// #include<any> Does not work on icpc...

class Argument {
    public:
    
    class RequiredException;

    Argument() {}
    Argument(boost::any default_value) : default_value(default_value) {}

    void set(const std::string& representation) { repr = representation; }

    template<typename T>
    T put(T& lvalue, const std::string& name = "", int index = -1);

    private:

    template<typename T>
    void cast_repr(T&);

    std::string repr;
    boost::any default_value;
};

#include"argument.hpp"

#endif // ARGUMENT_H
