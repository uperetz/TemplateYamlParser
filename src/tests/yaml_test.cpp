#include<iostream>
#include"yaml-parser/argument.h"

int main(int argc, char **argv) {
    YAML::Scalar<char> val;
    std::cout<<val<<"\n";
    val.set("a");
    std::cout<<val<<"\n";
    YAML::Sequence s(
        false, {},
        YAML::Scalar<char>{}, val, YAML::Scalar<std::string>{}
    );
    std::cout<<s<<"\n";

    return 0;
}
