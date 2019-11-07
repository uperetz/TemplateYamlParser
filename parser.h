#ifndef PARSER_H
#define PARSER_H 

#include"argument.h"
#include<vector>
#include<tuple>
#include<set>
#include<map>
// Syntactic sugar types

template<typename F, typename... Ts>
struct ArgumentPack {
    ArgumentPack(const F& func, Ts&... args) : run(func), args(args...) {}
    const F& run;
    const std::tuple<Ts&...> args;
};

typedef const std::set<std::string> Booleans;
typedef const std::set <std::string> RequiredNames;
typedef const std::map<std::string, std::any> OptionalNames;
typedef const std::vector<std::any> VecOptional;

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

    template<typename T>
    void put(const std::string& name, T& var);

    void parse_arguments(const std::vector<std::string>&);
    void parse_arguments(int argc, const char** argv);

    private:

    void valid_pos(size_t positional, VecOptional& vopt);

    void add(RequiredNames& names, size_t positional = 0, const VecOptional& vopt = VecOptional()); 
    void add(OptionalNames& names, size_t positional, const VecOptional& vopt); 
    
    Argument& find(const std::string name); 

    std::map<const std::string, Argument> argmap;
    std::vector<Argument> argvec;
    std::set<std::string> booleans;
};

#include"parser.hpp"

#endif /* PARSER_H */
