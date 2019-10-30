// Parser implementation
#ifndef PARSER_HPP
#define PARSER_HPP

#include"parser.h"

OptionalNames concatenate(OptionalNames& opt, Booleans booleans) {
    std::map<std::string, std::any> all(opt);
    for(auto name : booleans)
        all[name] = false;

    return all;
}

Parser::Parser(RequiredNames& req, size_t positional, VecOptional& vopt)
        : args(req, positional, vopt), booleans() {}

Parser::Parser(RequiredNames& req, OptionalNames& opt, size_t positional, VecOptional& vopt) 
        : args(req, opt, positional, vopt), booleans() {}

Parser::Parser(RequiredNames& req, OptionalNames& opt, Booleans& booleans, size_t positional,
               VecOptional& vopt)
        : args(req, concatenate(opt, booleans), positional, vopt), booleans(booleans) {}

Parser::Parser(OptionalNames& opt, size_t positional, VecOptional& vopt)
        : args(opt, positional, vopt), booleans() {}

Parser::Parser(OptionalNames& opt, Booleans& booleans, size_t positional, VecOptional& vopt)
        :  args(concatenate(opt, booleans), positional, vopt), booleans(booleans) {}

void Parser::parse_arguments(const std::vector<std::string>& arguments) {
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

void Parser::parse_arguments(int argc, const char** argv) {
    parse_arguments(std::vector<std::string>(argv + 1, argv + argc));
}
#endif /* PARSER_HPP */
