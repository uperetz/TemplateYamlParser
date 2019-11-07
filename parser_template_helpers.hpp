#ifndef ARGUMENT_TEMPLATE_HELPER_HPP
#define ARGUMENT_TEMPLATE_HELPER_HPP

#include"parser.hpp"

template<typename T, typename... Ts>
struct vec_collect_arguments {
    vec_collect_arguments(std::vector<Argument>::iterator pos,
                          std::vector<Argument>::iterator vec_end,
                          T& current, Ts&... args, unsigned int index = 0) {
        if (pos == vec_end)
            throw std::out_of_range("More variables than defined arguments!");
        pos->put(current, "", index);
        vec_collect_arguments<Ts...>(++pos, vec_end, args..., ++index);
    }
};

template<typename T>
struct vec_collect_arguments<T> {
    vec_collect_arguments(std::vector<Argument>::iterator pos,
                          std::vector<Argument>::iterator vec_end,
                          T& current, unsigned int index = 0) {
        if (pos+1 != vec_end)
            throw std::out_of_range("More arguments than variables!");
        pos->put(current, "", index);
    }
};

template<typename S, typename T, typename... Ts>
struct map_collect_arguments {
    map_collect_arguments(Parser& args, S& name, T& current,
                          Ts&... rest) {
        args.put(name, current);
        map_collect_arguments<Ts...>(args, rest...);
    }
};

template<typename S, typename T>
struct map_collect_arguments<S, T> {
    map_collect_arguments(Parser& args, S& name, T& current) {
        args.put(name, current);
    }
};

void get_names(std::vector<std::string>& help) {}

template<typename S, typename T, typename... Ts>
void get_names(std::vector<std::string>& help, const S& str, T& var, Ts... rest) {
    help.push_back(str);
    get_names(help, rest...);
}

auto get_variables(Parser& args) { return std::make_tuple(); }

template<typename S, typename T, typename... Ts>
auto get_variables(Parser& args, const S& str, T& var, Ts... rest) {
    args.put(str, var);
    return std::tuple_cat(std::make_tuple(var), get_variables(args, rest...));
}

template<typename T>
struct pack_collect_arguments {
    pack_collect_arguments(Parser& args, const T& argument_pack) {
        std::apply([&args](auto &... elements) {args.collect_named(elements...);}, argument_pack.args);
    }
};

template<typename T>
void runPack(Parser& args, std::vector<std::string>& help, T current) {
    std::apply([&help](auto &... v) {return get_names(help, v...);}, current.args);
    
    pack_collect_arguments(args, current);
    auto res = std::apply([&args](auto &... v) {return get_variables(args, v...);}, current.args);
    std::apply([current](auto &... v) { current.run(v...); }, res);
}

template<typename T, typename... Ts>
struct packs_collect_arguments {
    packs_collect_arguments(Parser& args, HelpGenerator packs_help, const T& current, 
                            const Ts&... arg_packs) {
        std::vector<std::string> help;
        try {
            runPack(args, help, current);
        } catch (const std::exception e) {
            packs_help.push_back(help);
            packs_collect_arguments<Ts...>(args, packs_help, arg_packs...);
        }
    }
};

#include<iostream>
template<typename T>
struct packs_collect_arguments<T> {
    packs_collect_arguments(Parser& args, HelpGenerator& packs_help, const T& current) {
        std::vector<std::string> help;
        try {
            runPack(args, help, current);
        } catch (const std::exception e) {
            packs_help.push_back(help);
            unsigned int count = 0;
            std::cout<<"-E- Could not fill any argument pack (missing required parameters). Options were:\n";
            for(auto pack_names : packs_help) {
                std::cout<<"Pack "<<count<<":\n    ";
                for(auto name : pack_names)
                    std::cout<<name<<" "; 
                std::cout<<"\n";
                ++count;
            }
            exit(1);
        }
    }
};
#endif /* ARGUMENT_TEMPLATE_HELPER_HPP */
