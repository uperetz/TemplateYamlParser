#ifndef PARSER_H
#define PARSER_H 

#include"argument.h"
#include"arg_path.h"
#include<yaml-cpp/yaml.h>
#include<memory>

namespace CMD {

template<typename F, typename... Ts>
struct ArgPack {
    ArgPack(F func, Ts... args) : args(std::make_tuple(args...)), func(func) {}
    std::tuple<Ts...> args;
    F func;
};

class Parser {
    public:
    /* constructors */
    Parser(const Sequence& args) : args(std::make_shared<Sequence>(args)) {}
    Parser(const Map& args) : args(std::make_shared<Map>(args)) {}
    Parser(const Scalar& args) : args(std::make_shared<Scalar>(args)) {}

    /* get arguments */
    void parse_argv(int argc, char **argv);
    void parse_yaml(const std::string& path);

    /* helper */
    void parse_yaml(const YAML::Node&, Sequence&);
    void parse_yaml(const YAML::Node&, Map&);
    void parse_yaml(const YAML::Node&, Scalar&);
   
    /* run functions */
    template<typename F, typename... Ts>
    void apply(const F& func, Ts&... ArgPaths);
    template<typename F, typename... Ts>
    void apply(ArgPack<F, Ts...>);
    template<typename... Ts>
    void apply_one(Ts... ArgPacks);

    private:
    std::shared_ptr<Argument> args;
};

} // CMD

#include"parser.hpp"

#endif /* PARSER_H */
