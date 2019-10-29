// Argument implementation.
#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP 

std::string debug_argument(const std::string& name = "", int index = -1) {
    return " faild on " + (name.empty() ?
                              (index > -1 ?
                                 "positional argument " + std::to_string(index)
                              : "")
                          : "argument '" + name + "'") + ".";
}

class Argument::RequiredException: public std::exception {
    public:

    RequiredException(const std::string& extra)
    : msg("Can't get argument with no default value without setting it first!" + extra) {}

    virtual const char* what() const throw () {
        return msg.c_str();
    }

    private:

    const std::string msg;
};

template<typename T>
T Argument::put(T& lvalue, const std::string& name, int index) {
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

template<typename T>
void Argument::cast_repr(T& lvalue) {
    std::stringstream(repr) >> lvalue;
}

#endif /* ARGUMENT_HPP */
