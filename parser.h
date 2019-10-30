#ifndef PARSER_H
#define PARSER_H 

#include"arguments.h"

class Parser {
    public:
    
    Parser(RequiredNames&, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(RequiredNames&, OptionalNames&, size_t positional = 0, VecOptional& = VecOptional());
    Parser(RequiredNames&, OptionalNames&, Booleans&, size_t positional = 0, VecOptional& = VecOptional());
    Parser(OptionalNames&, size_t positional = 0, VecOptional& vopt = VecOptional());
    Parser(OptionalNames&, Booleans&, size_t positional = 0, VecOptional& vopt = VecOptional());

    void parse_arguments(const std::vector<std::string>& arguments); 
    void parse_arguments(int argc, const char** argv);

    template<typename... Ts>
    void collect(Ts&... variables) { args.collect(variables...); }

    private:

    ArgList args;
    std::set<std::string> booleans;
};
    
#include"parser.hpp"

#endif /* PARSER_H */
