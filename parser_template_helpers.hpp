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

#include"icpc_helpers.hpp"

template<typename S, typename T, typename... Ts>
struct map_collect_arguments {
    map_collect_arguments(Parser& args, S& name, T& current,
                          Ts&... rest) {
        args.put(std::string(name), current);
        map_collect_arguments<Ts...>(args, rest...);
    }
};

template<typename S, typename T>
struct map_collect_arguments<S, T> {
    map_collect_arguments(Parser& args, S& name, T& current) {
        args.put(std::string(name), current);
    }
};

void get_names(HelpList& help) {}

template<typename S, typename T, typename... Ts>
void get_names(HelpList& help, const S& str, T& var, Ts... rest) {
    help.emplace_back(str);
    get_names(help, rest...);
}

auto get_variables(Parser& args) { return std::make_tuple(); }

template<typename S, typename T, typename... Ts>
auto get_variables(Parser& args, const S& str, T& var, Ts... rest) {
    args.put(std::string(str), var);
    return std::tuple_cat(std::make_tuple(var), get_variables(args, rest...));
}


template<typename T>
struct pack_collect_arguments {
    pack_collect_arguments(Parser& args, const T& argument_pack) {
        apply< std::tuple_size< decltype(argument_pack.args) >::value >(
            [&args](auto &... elements) {args.collect_named(elements...);}, argument_pack.args);
    }
};

template<typename T>
void runPack(Parser& args, HelpList& help, T current) {
    apply< std::tuple_size< decltype(current.args) >::value >(
        [&help](auto &... v) {return get_names(help, v...);}, current.args);
    
    pack_collect_arguments<T>(args, current);

    //auto res =  Will not work with icpc
    apply< std::tuple_size< decltype(current.args) >::value >(
        [&args](auto &... v) {return get_variables(args, v...);}, current.args);

    // Again, an icpc hack...
    apply_every_other< std::tuple_size< decltype(current.args) >::value >( current.run , current.args);
}

template<typename T, typename... Ts>
struct packs_collect_arguments {
    packs_collect_arguments(Parser& args, HelpGenerator& packs_help, const T& current, 
                            const Ts&... arg_packs) {
        HelpList help;
        try {
            runPack(args, help, current);
        } catch (const std::exception e) {
            packs_help.push_back(help);
            packs_collect_arguments<Ts...>(args, packs_help, arg_packs...);
        }
    }
};

template<typename T>
struct packs_collect_arguments<T> {
    packs_collect_arguments(Parser& args, HelpGenerator& packs_help, const T& current) {
        HelpList help;
        try {
            runPack(args, help, current);
        } catch (const std::exception e) {
            packs_help.push_back(help);
            unsigned int count = 0;
            std::stringstream error;
            error<<"\n-E- Could not fill any argument pack (missing required parameters). Options were:\n";
            for(auto pack_names : packs_help) {
                error<<"Pack "<<count<<":\n    ";
                for(auto name : pack_names)
                    error<<name<<" "; 
                error<<"\n";
                ++count;
            }
            throw std::invalid_argument(error.str());
        }
    }
};
#endif /* ARGUMENT_TEMPLATE_HELPER_HPP */
