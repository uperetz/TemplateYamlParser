#include"cmd-parser/parser.h"
#include<iostream>

int main(int argc, char **argv) {
    CMD::Parser parser(CMD::RequiredNames{"c"}, CMD::OptionalNames{{"aba", 3}});
    parser.parse_argv_yaml(argc, argv);
    int aba;
    char c;
    parser.collect_named("aba", aba, "c", c);
    std::cout<<aba<<" "<<c<<"\n";

    return 0;
}
