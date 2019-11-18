// Parser implementation
#ifndef PARSER_HPP
#define PARSER_HPP

#include"parser.h"

namespace CMD {

template<typename T, typename C, typename... Ts>
void parse_argument(const T& next, CMD::Argument& arg, C& control, Ts&... extraArgs) {
    Sequence* seq = NULL;
    Map* map = NULL;
    Scalar* scalar = NULL;
    if(next.IsSequence()) {
        if(!(seq = dynamic_cast<Sequence*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Sequence!");
        control.parse_yaml(next, *seq, extraArgs...);
    } else if(next.IsMap()) {
        if(!(map = dynamic_cast<Map*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Map!");
        control.parse_yaml(next, *map, extraArgs...);
    } else if(next.IsScalar()) {
        if(!(scalar = dynamic_cast<Scalar*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Scalar!");
        control.parse_yaml(next, *scalar, extraArgs...);
    } else {
        throw std::invalid_argument("-E- Badly formed Yaml! Not one of Map, Sequence, or Scalar.");
    }
}

/* Constructors */
void Parser::parse_yaml(const YAML::Node& root, Sequence& current) {
    size_t index = 0;
    for(auto it = root.begin(); it != root.end(); ++it)
        parse_argument(*it, current[index++], *this);
}

void Parser::parse_yaml(const YAML::Node& root, Map& current) {
    for(auto it = root.begin(); it != root.end(); ++it)
        parse_argument(it->second, current[it->first.Scalar()], *this);
}

void Parser::parse_yaml(const YAML::Node& root, Scalar& current) {
    current.set_repr(root.Scalar());
}

void Parser::parse_yaml(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    parse_argument(root, *args, *this);
}

void Parser::parse_argv(int argc, char **argv) {
    if(argc != 3 || std::string(argv[1]) != "-y")
        throw std::invalid_argument("-E- Usage: program -y /path/to/yaml");
    parse_yaml(argv[2]);
}

template<typename T>
T activate_parse(ArgPath<T>& arg_path, Argument& args) {
    // return arg_path.parse_yaml(args);
    T var;
    parse_argument(arg_path, args, arg_path, var);
    return var;
}

template<typename F, typename... Ts>
void Parser::apply(const F& func, Ts&... ArgPaths) {
    std::apply(func, std::make_tuple(activate_parse(ArgPaths, *args)...));
}

template<typename F, typename... Ts>
void Parser::apply(ArgPack<F, Ts...> ArgPaths) {
    std::apply([this, &ArgPaths](auto &... elements) {apply(ArgPaths.func, elements...);},
        ArgPaths.args);
}

template<typename T>
void apply_one_helper(Parser& p, T& arg_pack) {
    try {
        p.apply(arg_pack);
    } catch(std::invalid_argument) {
        throw std::invalid_argument("-E- Failed all argument packs!");
    }
}

template<typename T, typename... Ts>
void apply_one_helper(Parser& p, T& arg_pack, Ts... ArgPacks) {
    try {
        p.apply(arg_pack);
    } catch(std::invalid_argument) {
        apply_one_helper(p, ArgPacks...);
    }
}

template<typename... Ts>
void Parser::apply_one(Ts... ArgPacks) {
    apply_one_helper(*this, ArgPacks...);
}

} // CMD

#endif /* PARSER_HPP */
