#ifndef ARG_PATH_H
#define ARG_PATH_H

#include"argument.h"

namespace CMD {
// Variant linked list
template<typename T>
class ArgPath {
    public: 
    enum class Type {Index, Key, Leaf};

    /* constructors */
    ArgPath() : type(Type::Leaf) {}
    template<typename... Ts>
    ArgPath(const size_t& index, const Ts&... rest);
    template<typename... Ts>
    ArgPath(const std::string& key, const Ts&... rest);

    /* type */
    bool IsSequence() const {return type == Type::Index;}
    bool IsMap() const {return type == Type::Key;}
    bool IsScalar() const {return type == Type::Leaf;}
   
    /* getters */
    size_t get_index() const;
    const std::string& get_key() const;
    const ArgPath& get_next() const;
   
    /* get arguments */
    template<typename A,
             typename = typename std::enable_if<std::is_base_of<Argument, A>::value>::type>
    void parse_yaml(A&, T& var);
    template<typename A,
             typename = typename std::enable_if<std::is_base_of<Argument, A>::value>::type>
    T parse_yaml(A&);
    
    /* helper */
    void parse_yaml(const ArgPath&, Sequence&, T& var);
    void parse_yaml(const ArgPath&, Map&, T& var);
    void parse_yaml(const ArgPath&, Scalar&, T& var);
    
    private:
    Type type;
    size_t index;
    std::string key;
    std::shared_ptr<ArgPath> next = NULL;
};

} // CMD

#include"arg_path.hpp"

#endif /* ARG_PATH_H */
