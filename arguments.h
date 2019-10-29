#ifndef ARGUMENT_H
#define ARGUMENT_H

#include<string>
#include<vector>
#include<any>
#include<map>
#include<set>

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

// Syntactic sugar types
typedef const std::set<std::string> Booleans;
typedef const std::set <std::string> RequiredNames;
typedef const std::map<std::string, std::any> OptionalNames;
typedef const std::vector<std::any> VecOptional;

class ArgList  {
    public:

    ArgList(OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(opt, positional, vopt);
    }
    ArgList(RequiredNames& req, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(req, positional, vopt);
    }
    ArgList(RequiredNames& req, OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(opt, positional, vopt);
        add(req);
    }

    void set(const std::string& name, const std::string& value); 
    void set(unsigned int index, const std::string& value);

    template<typename... Ts>
    void collect(Ts&... lvalues);
    
    private:

    void valid_pos(size_t positional, VecOptional& vopt);

    void add(RequiredNames& names, size_t positional = 0, const VecOptional& vopt = VecOptional()); 
    void add(OptionalNames& names, size_t positional, const VecOptional& vopt); 
    
    Argument& find(const std::string name); 

    std::map<const std::string, Argument> argmap;
    std::vector<Argument> argvec;
};

#include"arglist.hpp"

#endif // ARGUMENT_H
