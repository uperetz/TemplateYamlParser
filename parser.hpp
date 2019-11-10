// Parser implementation
#ifndef PARSER_HPP
#define PARSER_HPP

#include"parser.h"
#include<algorithm>

OptionalNames add_bool_options(Booleans bools, OptionalNames& opt = OptionalNames()) {
    std::unordered_map<std::string, std::any> all(opt); 
    for(auto name : bools)
        all[name] = false;
    return all;
}

Parser::Parser(RequiredNames& req, size_t positional, VecOptional& vopt) : argvec(positional) {
    add(req, positional, vopt);
}

Parser::Parser(RequiredNames& req, Booleans& bools, size_t positional, VecOptional& vopt)
               : argvec(positional), booleans(bools) {
    OptionalNames opt(add_bool_options(bools));
    add(opt, positional, vopt);
    add(req);
}

Parser::Parser(RequiredNames& req, OptionalNames& opt, size_t positional, VecOptional& vopt) 
               : argvec(positional) {
    add(opt, positional, vopt);
    add(req);
}

Parser::Parser(RequiredNames& req, OptionalNames& opt, Booleans& bools, size_t positional,
               VecOptional& vopt) : argvec(positional), booleans(bools) {
    OptionalNames combined(add_bool_options(bools, opt));
    add(combined, positional, vopt);
    add(req);
}

Parser::Parser(OptionalNames& opt, size_t positional, VecOptional& vopt) : argvec(positional) {
    add(opt, positional, vopt);
}

Parser::Parser(OptionalNames& opt, Booleans& bools, size_t positional, VecOptional& vopt)
                 : argvec(positional), booleans(bools) {
    OptionalNames combined(add_bool_options(bools, opt));
    add(combined, positional, vopt);
}

void Parser::set(const std::string& name, const std::string& value) {
    find(name).set(value);
}

void Parser::set(unsigned int index, const std::string& value) {
    if (index >= argvec.size())
        throw std::invalid_argument("index " + std::to_string(index)+ " out of bounds for positional arguments!");
    argvec[index].set(value);
}

typedef std::vector<std::vector<std::string>> HelpGenerator;

template<typename S, typename T, typename... Ts>
struct map_collect_arguments;

template<typename T, typename... Ts>
struct packs_collect_arguments;

template<typename... Ts>
void Parser::collect_named(Ts&... name_lvalue_pairs) {
    map_collect_arguments<Ts...>(*this, name_lvalue_pairs...);
}

template<typename... Ts>
void Parser::run_one_of(const Ts&... argument_packs) {
    HelpGenerator helper; 
    packs_collect_arguments<Ts...>(*this, helper, argument_packs...);
}

template<typename T, typename... Ts>
struct vec_collect_arguments;
    
template<typename... Ts>
void Parser::collect_positional(Ts&... lvalues) {
    vec_collect_arguments<Ts...>(argvec.begin(), argvec.end(), lvalues...);
}

void Parser::valid_pos(size_t positional, VecOptional& vopt) {
    if (vopt.size() > positional)
        throw std::out_of_range("Amount of optional postional arguments greater than amount of total.");
}

void Parser::add(RequiredNames& names, size_t positional, const VecOptional& vopt) {
    valid_pos(positional, vopt);
    for (auto name : names) {
        argmap[name] = Argument();
        required.insert(name);
    }
}

void Parser::add(OptionalNames& names, size_t positional, const VecOptional& vopt) {
    valid_pos(positional, vopt);
    for (auto name_it : names)
        argmap[name_it.first] = Argument(name_it.second);
    auto opt_iter = vopt.begin();
    for (auto iter = argvec.begin()+(positional-vopt.size()); iter != argvec.end(); ++iter, ++opt_iter)
        (*iter) = Argument(*opt_iter);
}

Argument& Parser::find(const std::string name) {
    auto arg_it = argmap.find(name);
    if (arg_it == argmap.end())
        throw std::invalid_argument("argument '" + name + "' not found in argument list!");
    return arg_it->second;
}

template<typename T>
void Parser::put(const std::string& name, T& current) {
    auto param = argmap.find(name);
    if(param == argmap.end())
        throw std::invalid_argument("Did not find " + name + " in argument list!");
    (param->second).put(current, name);
}

template<typename T>
T Parser::get(const std::string& name) {
    T var;
    put(name, var);
    return var;
}

void Parser::parse_arguments(const std::vector<std::string>& arguments, bool validate) {
    std::string name;
    int index = 0;
    std::unordered_set<std::string> required_found;

    for (auto arg : arguments) {
        if (!name.empty()) {
            if (arg[0] == '-')
                throw std::invalid_argument(name + " expected an argument, but was followed with " + arg);      
            set(name, arg);
            if(validate && required.count(name))
                required_found.insert(name);
            name = "";
        } else if (arg[0] == '-') {
            arg.erase(0, arg.find_first_not_of('-'));
            if (! booleans.count(arg))
                name = arg;
            else
                set(arg, "1");
        } else {
            try {
                set(index++, arg);
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("More positional arguments given than declared!");
            }
        }
    }
    if (!name.empty())
        throw std::invalid_argument(name + " expected an argument, but none are left!");
    if(validate && required_found.size() != required.size()) {
        std::string error("-E- Missing required arguments: ");
        for(std::string name : required)
            if( !required_found.count(name) )
                error += name + " ";
        throw std::invalid_argument(error);
    }
}

void Parser::parse_arguments(int argc, char** argv, bool validate) {
    parse_arguments(std::vector<std::string>(argv + 1, argv + argc), validate);
}

#include"parser_template_helpers.hpp"

#endif /* PARSER_HPP */
