#ifndef ARGUMENT_H
#define ARGUMENT_H 

#include<string>
#include<initializer_list>
#include<memory>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<iostream>
#include<any>

namespace CMD {

using Dependencies = const std::vector<std::string>;

class Argument {
    public:
    enum class Type {SEQ, MAP, VAL};

    Argument(Dependencies& deps) : dependencies(deps) {}
   
    virtual Type type() = 0;
    virtual std::ostream& print(std::ostream& os, const std::string& spaces = std::string()) = 0;
    
    void print_deps() { for(auto d : dependencies) std::cout<<d<<"\n";}

    protected:
    Dependencies dependencies;
};

class Sequence : public Argument {
    public:
    template<typename... Ts>
    Sequence(Dependencies& dependencies, Ts&&... args);

    virtual Type type() {return Type::SEQ;}
    virtual std::ostream& print(std::ostream& os, const std::string& spaces = std::string());
    
    Argument& operator[](size_t index);

    private:
    std::vector<std::shared_ptr<Argument>> args;
};

class Map : public Argument {
    public:
    template<typename... Ts>
    Map(Dependencies& dependencies, Ts&&... args);

    virtual Type type() {return Type::MAP;}
    virtual std::ostream& print(std::ostream& os, const std::string& spaces = std::string());
    
    Argument& operator[](const std::string& key);
    Argument& operator[](std::nullptr_t) = delete;

    private:
    std::unordered_map<std::string, std::shared_ptr<Argument>> args;
};

class Scalar : public Argument {
    public:
    Scalar(Dependencies& deps = Dependencies()) : Argument(deps) {}
    Scalar(const char*  default_val, Dependencies& deps = Dependencies())
           : Argument(deps), val(std::string(default_val)) {}
    template<typename T>
    Scalar(T default_val, Dependencies& deps = Dependencies()) : Argument(deps), val(default_val) {}
    
    virtual Type type() {return Type::VAL;}
    virtual std::ostream& print(std::ostream& os, const std::string& spaces = std::string());
  
    void set_repr(const std::string& repr);
    template<typename T>
    void set(const std::string& repr);
    template<typename T>
    T get(T&);
    const std::string& get_repr() {return repr;}

    private:
    std::any val;
    std::string repr;
};

} // CMD

#include"./argument.hpp"

#endif /* ARGUMENT_H */
