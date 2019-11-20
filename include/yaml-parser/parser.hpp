// Parser implementation
#ifndef PARSER_HPP
#define PARSER_HPP

#include"parser.h"

namespace CMD {

/* Parse */
void Parser::parse_yaml(const YAML::Node& root, Sequence& current) {
    size_t index = 0;
    for(auto it = root.begin(); it != root.end(); ++it)
        parse_argument(*it, current[index++]);
}

void Parser::parse_yaml(const YAML::Node& root, Map& current) {
    for(auto it = root.begin(); it != root.end(); ++it)
        parse_argument(it->second, current[it->first.Scalar()]);
}

void Parser::parse_yaml(const YAML::Node& root, Scalar& current) {
    current.set_repr(root.Scalar());
}

void Parser::parse_yaml(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    parse_argument(root, *args);
}

void Parser::parse_argv(int argc, char **argv) {
    if(argc != 3 || std::string(argv[1]) != "-y")
        throw std::invalid_argument("-E- Usage: program -y /path/to/yaml");
    parse_yaml(argv[2]);
}

template<typename T>
void Parser::parse_argument(const T& next, CMD::Argument& arg) {
    Sequence* seq = NULL;
    Map* map = NULL;
    Scalar* scalar = NULL;
    if(next.IsSequence()) {
        if(!(seq = dynamic_cast<Sequence*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Sequence!");
        this->parse_yaml(next, *seq);
    } else if(next.IsMap()) {
        if(!(map = dynamic_cast<Map*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Map!");
        this->parse_yaml(next, *map);
    } else if(next.IsScalar()) {
        if(!(scalar = dynamic_cast<Scalar*>(&arg)))
            throw std::invalid_argument("-E- Argument does not match Yaml, expected Scalar!");
        this->parse_yaml(next, *scalar);
    } else {
        throw std::invalid_argument("-E- Badly formed Yaml! Not one of Map, Sequence, or Scalar.");
    }
}

/* Apply */
template<typename T>
T activate_parse(const ArgPath<T>& arg_path, Argument& args) {
    return arg_path.parse_argument(arg_path, args);
}

template<typename F, typename... Ts>
void Parser::apply(const F& func, const Ts... ArgPaths) const {
    std::apply(func, std::make_tuple(activate_parse(ArgPaths, *args)...));
}

template<typename F, typename... Ts>
void Parser::apply(const ArgPack<F, Ts...> ArgPaths) const {
    std::apply([this, &ArgPaths](auto &... elements) {apply(ArgPaths.func, elements...);},
               ArgPaths.args);
}

template<typename T>
void apply_one_helper(const Parser& p, const T& arg_pack) {
    try {
        p.apply(arg_pack);
    } catch(std::invalid_argument) {
        throw std::invalid_argument("-E- Failed all argument packs!");
    }
}

template<typename T, typename... Ts>
void apply_one_helper(const Parser& p, const T& arg_pack, const Ts... ArgPacks) {
    try {
        p.apply(arg_pack);
    } catch(std::invalid_argument) {
        apply_one_helper(p, ArgPacks...);
    }
}

template<typename... Ts>
void Parser::apply_one(const Ts... ArgPacks) const {
    apply_one_helper(*this, ArgPacks...);
}

template<typename F, typename... Ts>
void Parser::apply_to_all(const F& func, const ArgPath<CMD::Sequence&>& argpath, const ArgPath<Ts>&... ArgPaths) const {
    Sequence& argseq = argpath.parse_argument(argpath, *args);
    for(auto&& p_iter : argseq)
        std::apply(func, std::make_tuple(activate_parse(ArgPaths, *p_iter)...));
}

template<typename F, typename... Ts>
void Parser::apply_to_all(const F& func, const ArgPath<CMD::Map&>& argpath, const ArgPath<Ts>&... ArgPaths) const {
    Map& argmap = argpath.parse_argument(argpath, *args);
    for(auto&& p_iter : argmap)
        std::apply(func, std::make_tuple(activate_parse(ArgPaths, *(p_iter.second))...));
}

} // CMD

#endif /* PARSER_HPP */
