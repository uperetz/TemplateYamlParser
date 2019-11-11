#ifndef PARSER_H
#define PARSER_H 

#include"argument.h"
#include<vector>
#include<tuple>
#include<unordered_set>
#include<unordered_map>
#include<functional> // To make icpc work...
// Syntactic sugar types

template<typename F, typename... Ts>
struct ArgumentPack {
    ArgumentPack(const F& func, Ts&... args) : run(func), args(args...) {
            //std::get<0>(this->args) = 3; 
        }
    const F& run;
    const std::tuple<std::reference_wrapper<Ts>...> args;
};

typedef const std::unordered_set<std::string> Booleans;
typedef const std::unordered_set <std::string> RequiredNames;
typedef const std::unordered_map<std::string, boost::any> OptionalNames;
typedef const std::vector<boost::any> VecOptional;

class Parser {
    public:

    Parser(RequiredNames&, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(RequiredNames&, Booleans&, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(RequiredNames&, OptionalNames&, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(RequiredNames&, OptionalNames&, Booleans&, size_t positional = 0, 
            VecOptional& vopt = VecOptional());
    Parser(OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(OptionalNames& opt, Booleans&, size_t positional = 0, VecOptional& vopt = VecOptional());

    void set(const std::string& name, const std::string& value); 
    void set(unsigned int index, const std::string& value);
    
    template<typename... Ts>
    void run_one_of(const Ts&... argument_packs);
    
    template<typename... Ts>
    void collect_named(Ts&... names_lvalue_pairs);
    
    template<typename... Ts>
    void collect_positional(Ts&... lvalues);

    // icpc compatibility
    template<typename T>
    void put(const std::string& name, std::reference_wrapper<T> current);

    template<typename T>
    void put(const std::string& name, T& var);
   
    template<typename T>
    T get(const std::string& name);

    void parse_arguments(const std::vector<std::string>&, bool validate=true);
    void parse_arguments(int argc, char** argv, bool validate=true);

    private:

    void valid_pos(size_t positional, VecOptional& vopt);

    void add(RequiredNames& names, size_t positional = 0, const VecOptional& vopt = VecOptional()); 
    void add(OptionalNames& names, size_t positional, const VecOptional& vopt); 
    
    Argument& find(const std::string name); 

    std::unordered_map<std::string, Argument> argmap;
    std::vector<Argument> argvec;
    std::unordered_set<std::string> booleans;
    std::unordered_set<std::string> required;
};

#include"parser.hpp"

#endif /* PARSER_H */
