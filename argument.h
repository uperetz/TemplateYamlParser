#ifndef ARGUMENT_H
#define ARGUMENT_H

#include<string>
#include<vector>
#include<any>
#include<map>
#include<set>

std::string debug_argument(const std::string& name = "", int index = -1) {
    return " faild on " + (name.empty() ? 
                              (index > -1 ?
                                 "positional argument " + std::to_string(index)
                              : "")
                          : "argument '" + name + "'") + ".";
}

class Argument {
    public:
    class RequiredException: public std::exception {
        public:
    
        RequiredException(const std::string& extra) 
        : msg("Can't get argument with no default value without setting it first!" + extra) {}
    
        virtual const char* what() const throw () {
            return msg.c_str();
        }
    
        private:
    
        const std::string msg;
    };

    Argument() {}
    Argument(std::any default_value) : default_value(default_value) {}

    void set(const std::string& representation) {
        repr = representation;
    }

    template<typename T>
    T put(T& lvalue, const std::string& name = "", int index = -1) {
        if (repr.empty())
            if (default_value.has_value())
                try { 
                    lvalue = std::any_cast<T>(default_value);
                } catch (std::bad_any_cast e) {
                    throw std::invalid_argument("Trying to derefence argument default value with wrong value type. Make sure the type matches given default value!" + debug_argument(name, index));
                }
            else
                throw RequiredException(debug_argument(name, index));
        cast_repr<T>(lvalue);
        return lvalue;
    }
    
    private:
    
    template<typename T>
    void cast_repr(T& lvalue) {
        std::stringstream(repr) >> lvalue;
    }

    std::string repr;
    std::any default_value;
};

typedef const std::set<std::string> Booleans;
typedef const std::set <std::string> RequiredNames;
typedef const std::map<std::string, std::any> OptionalNames;
typedef const std::vector<std::any> VecOptional;

template<typename T, typename... Ts>
struct collect_arguments;

class ArgList  {
    public:

    ArgList(OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(opt, positional, vopt);
    }
    ArgList(RequiredNames& req, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(req, positional, vopt);
    }
    ArgList(RequiredNames& req, OptionalNames& opt, size_t positional = 0, VecOptional& vopt = VecOptional()) : argvec(positional) {
        add(opt, positional, vopt);
        add(req);
    }

    void set(const std::string& name, const std::string& value) {
        find(name).set(value);
    }
    
    void set(unsigned int index, const std::string& value) {
        if (index >= argvec.size())
            throw std::invalid_argument("index " + std::to_string(index)+ " out of bounds for positional arguments!");
        argvec[index].set(value);
    }

    template<typename... Ts>
    void collect(Ts&... lvalues) {
        if (sizeof...(Ts) != argmap.size() + argvec.size())
            throw std::out_of_range("Amount of defined arguments does not match amount of given variables!");
        collect_arguments<Ts...>(argmap.begin(), argmap.end(), argvec.begin(), argvec.end(), lvalues...);
    }
    
    private:

    void valid_pos(size_t positional, VecOptional& vopt) {
        if (vopt.size() > positional)
            throw std::out_of_range("Amount of optional postional arguments greater than amount of total.");
    }       

    void add(RequiredNames& names, size_t positional = 0, const VecOptional& vopt = VecOptional()) {
        valid_pos(positional, vopt);
        for (auto name : names)
            argmap[name] = Argument();
    }
    
    void add(OptionalNames& names, size_t positional, const VecOptional& vopt) {
        valid_pos(positional, vopt);
        for (auto name_it : names)
            argmap[name_it.first] = Argument(name_it.second);
        auto opt_iter = vopt.begin();
        for (auto iter = argvec.begin()+(positional-vopt.size()); iter != argvec.end(); ++iter, ++opt_iter)
            (*iter) = Argument(*opt_iter);
    }
    
    Argument& find(const std::string name) {
        auto arg_it = argmap.find(name);
        if (arg_it == argmap.end())
            throw std::invalid_argument("argument '" + name + "' not found in argument list!");
        return arg_it->second;
    }


    std::map<const std::string, Argument> argmap;
    std::vector<Argument> argvec;
};

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

#endif // ARGUMENT_H
