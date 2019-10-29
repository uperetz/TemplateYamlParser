// ArgList implementation
#ifndef ARGLIST_HPP
#define ARGLIST_HPP

void ArgList::set(const std::string& name, const std::string& value) {
    find(name).set(value);
}

void ArgList::set(unsigned int index, const std::string& value) {
    if (index >= argvec.size())
        throw std::invalid_argument("index " + std::to_string(index)+ " out of bounds for positional arguments!");
    argvec[index].set(value);
}

template<typename T, typename... Ts>
struct collect_arguments;

template<typename... Ts>
void ArgList::collect(Ts&... lvalues) {
    if (sizeof...(Ts) != argmap.size() + argvec.size())
        throw std::out_of_range("Amount of defined arguments does not match amount of given variables!");
    collect_arguments<Ts...>(argmap.begin(), argmap.end(), argvec.begin(), argvec.end(), lvalues...);
}

void ArgList::valid_pos(size_t positional, VecOptional& vopt) {
    if (vopt.size() > positional)
        throw std::out_of_range("Amount of optional postional arguments greater than amount of total.");
}

void ArgList::add(RequiredNames& names, size_t positional, const VecOptional& vopt) {
    valid_pos(positional, vopt);
    for (auto name : names)
        argmap[name] = Argument();
}

void ArgList::add(OptionalNames& names, size_t positional, const VecOptional& vopt) {
    valid_pos(positional, vopt);
    for (auto name_it : names)
        argmap[name_it.first] = Argument(name_it.second);
    auto opt_iter = vopt.begin();
    for (auto iter = argvec.begin()+(positional-vopt.size()); iter != argvec.end(); ++iter, ++opt_iter)
        (*iter) = Argument(*opt_iter);
}

Argument& ArgList::find(const std::string name) {
    auto arg_it = argmap.find(name);
    if (arg_it == argmap.end())
        throw std::invalid_argument("argument '" + name + "' not found in argument list!");
    return arg_it->second;
}

template<typename T, typename... Ts>
struct vec_collect_arguments {
    vec_collect_arguments(std::vector<Argument>::iterator pos,
                          std::vector<Argument>::iterator vec_end,
                          unsigned int index, T& current, Ts&... args) {
        if (pos == vec_end)
            throw std::out_of_range("More variables than defined arguments!");
        pos->put(current, "", index);
        vec_collect_arguments<Ts...>(++pos, vec_end, ++index, args...);
    }
};

template<typename T>
struct vec_collect_arguments<T> {
    vec_collect_arguments(std::vector<Argument>::iterator pos,
                          std::vector<Argument>::iterator vec_end,
                          unsigned int index, T& current) {
        if (pos+1 != vec_end)
            throw std::out_of_range("More arguments than variables!");
        pos->put(current, "", index);
    }
};

template<typename T, typename... Ts>
struct collect_arguments {
    collect_arguments(std::map<std::string, Argument>::iterator name,
                      std::map<std::string, Argument>::iterator map_end,
                      std::vector<Argument>::iterator pos,
                      std::vector<Argument>::iterator vec_end,
                      T& current, Ts&... args) {
        if (name == map_end) {
            vec_collect_arguments<T, Ts...>(pos, vec_end, 0, current, args...);
            return;
        }
        name->second.put(current, name->first);
        collect_arguments<Ts...>(++name, map_end, pos, vec_end, args...);
    }
};

template<typename T>
struct collect_arguments<T> {
    collect_arguments(std::map<std::string, Argument>::iterator name,
                      std::map<std::string, Argument>::iterator map_end,
                      std::vector<Argument>::iterator pos,
                      std::vector<Argument>::iterator vec_end,
                      T& current) {
        if (name == map_end) {
            vec_collect_arguments<T>(pos, vec_end, 0, current);
            return;
        }
        name->second.put(current, name->first);
    }
};
#endif /* ARGLIST_HPP */
