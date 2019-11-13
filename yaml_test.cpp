#include"parser.h"
#include<iostream>

int main(int argc, char **argv) {
    Parser parser(RequiredNames{"c"}, OptionalNames{{"aba", 3}});
    parser.parse_argv_yaml(argc, argv);
    int aba;
    char c;
    parser.collect_named("aba", aba, "c", c);
    std::cout<<aba<<" "<<c<<"\n";

    return 0;
}
