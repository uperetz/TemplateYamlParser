#ifndef PARSER_H
#define PARSER_H 

#include"argument.h"
#include<iostream>
#include<exception>

OptionalNames concatenate(OptionalNames&, Booleans);

class Parser {
    public:
    Parser(RequiredNames& req, size_t positional = 0, VecOptional& vopt = VecOptional())
          : args(req, positional, vopt), booleans() {}

    Parser(RequiredNames& req, OptionalNames& opt, size_t positional = 0, 
           VecOptional& vopt = VecOptional()) : args(req, opt, positional, vopt), booleans() {}

    Parser(RequiredNames& req, OptionalNames& opt, Booleans& booleans, size_t positional = 0, 
           VecOptional& vopt = VecOptional()) 
          : args(req, concatenate(opt, booleans), positional, vopt), booleans(booleans) {}

    Parser(OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional()) 
          : args(opt, positional, vopt), booleans() {}

    Parser(OptionalNames& opt, Booleans& booleans, size_t positional = 0, VecOptional& vopt = VecOptional())
          :  args(concatenate(opt, booleans), positional, vopt), booleans(booleans) {}

    void parse_arguments(const std::vector<std::string>& arguments) {
        std::string name;
        int index = 0;
        for (auto arg : arguments) {
            if (!name.empty()) {
                if (arg[0] == '-')
                    throw std::invalid_argument(name + " expected an argument, but was followed with " + arg);
                args.set(name, arg);
                name = "";
            } else if (arg[0] == '-') {
                arg.erase(0, arg.find_first_not_of('-'));
                if (! booleans.count(arg))
                    name = arg;
                else
                    args.set(arg, "1"); 
            } else {
                try {
                    args.set(index++, arg);
                } catch (const std::invalid_argument& e) {
                    throw std::invalid_argument("More positional arguments given than declared!");
                }
            }
        }
        if (!name.empty())
            throw std::invalid_argument(name + " expected an argument, but none are left!");
    }

    template<typename... Ts>
    void collect(Ts&... variables) { args.collect(variables...); }

    private:

    ArgList args;
    Booleans booleans;
};
    
OptionalNames concatenate(OptionalNames& opt, Booleans booleans) {
    std::map<std::string, std::any> all(opt);
    for(auto name : booleans)
        all[name] = false;

    return all;
}


#endif /* PARSER_H */
