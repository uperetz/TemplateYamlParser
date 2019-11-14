#ifndef ARGUMENT_H
#define ARGUMENT_H 

#include<string>
#include<initializer_list>
#include<memory>
#include<vector>
#include<iostream>

namespace YAML {

using Dependencies = const std::vector<std::string>;
    
class Argument {
    public:
    enum class Type {SEQ, MAP, VAL};

    Argument(bool required, Dependencies& deps) : required(required), dependencies(deps) {}
   
    virtual Type type() = 0;
    template<typename T>
    T get() {}
    
    template<typename T>
    T& begin();
    template<typename T>
    T& end();

    bool is_required() {return required;}
    void print_deps() { for(auto d : dependencies) std::cout<<d<<"\n";}
    virtual std::ostream& print(std::ostream& os) = 0;

    friend std::ostream& operator<<(std::ostream& os, Argument& arg);

    protected:
    bool required = true;
    Dependencies dependencies;
};

class Sequence : public Argument {
    public:
    template<typename... Ts>
    Sequence(bool required, Dependencies& dependencies, const Ts&... args);

    virtual Type type() {return Type::SEQ;}
    virtual std::ostream& print(std::ostream& os);

    private:
    std::vector<std::unique_ptr<Argument>> args;
};

template<typename T>
class Scalar : public Argument {
    public:
    Scalar(bool required = true, Dependencies& deps = Dependencies()) : Argument(required, deps) {}
    
    virtual Type type() {return Type::VAL;}
    virtual std::ostream& print(std::ostream& os);
    T get();
    void set(const std::string& value);

    bool isnull() {return null;}
    Scalar<T>& operator=(const Scalar<T>& other);

    T val;
    bool null = true;
};

} // YAML

#include"./argument.hpp"

#endif /* ARGUMENT_H */
