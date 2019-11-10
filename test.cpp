#include<iostream>
#include<any>
#include"parser.h"
#include<stdint.h>
#include<string>
#include<vector>

void f1(int x, char y) {
    std::cout<<"Ran f1("<<x<<", "<<y<<")\n";
}
void f2(uint32_t x, std::string& y) {
    std::cout<<"Ran f2("<<x<<", "<<y<<")\n";
}

int main(int argc, char* argv[]) {
    int x;
    uint32_t y;
    std::string z;
    char w;

    uint64_t a;
    bool b;
    std::string c;
    int d;
    char e;
   
    RequiredNames req{"int", "string", "uint32_t"};
    OptionalNames opt{ {"char", 'c'} };
    Parser argm(req, opt, 5, {std::string("hi"), 21, 'a'});

    Booleans bla{"abc", "def"};
    Booleans bl(bla);

    // Comment one of me to see required errors
    argm.set("int", "3");
    argm.set("uint32_t", "123");
    argm.set("string", "yay");
    
    argm.set(0, "321");
    argm.set(1, "1");

    argm.run_one_of(
        ArgumentPack(f1, "int", x, 
                         "char", w),
        ArgumentPack(f2, "uint32_t", y,
                         "string", z)
    );
    
    // argm.set("uint64_t", "21"); Uncomment to see not in argument list
    argm.collect_named("string", c,
                       //"uint64_t", a,
                       "char", e,
                       "int", d);
    std::cout<<" : "<<" : "<<e<<" : "<<d<<"\n";

    std::cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";

    Parser p = Parser( RequiredNames{"int", "string", "uint32_t"},
                       OptionalNames{ {"std::string", std::string("Hello world")} },
                       Booleans{"bool"}, 3);//, VecOptional{std::string("astring")});

    std::string f;

    p.parse_arguments({"a", "-bool", "-uint32_t", "23", "-int", "213", "mc", "-string", "aba", "23"});
    p.collect_named("bool", b,
                    "int", x,
                    "string", z, 
                    "uint32_t", y);
    std::cout<<b<<" "<<x<<" "<<z<<" "<<y<<"\n";
    p.collect_positional(e, f, a);
    std::cout<<e<<" "<<f<<" "<<a<<"\n";
    std::cout<<p.get<bool>("bool")<<" And "<<p.get<std::string>("string")<<"\n";
    
    std::cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";
    // ./test -int 3 -string ab -uint32_t 101 aa 2 a
    p = Parser( RequiredNames{"int", "string", "uint32_t"},
                OptionalNames{ {"std::string", std::string("Hello world")} },
                Booleans{"bool"}, 3, VecOptional{std::string("astring")});
    p.parse_arguments(argc, argv, false); // Remove false for validation.
    p.collect_named("bool", b,
                    "int", x,
                    "string", z, 
                    "uint32_t", y);
    std::cout<<b<<" "<<x<<" "<<z<<" "<<y<<"\n";
    p.collect_positional(e, f, a);
    std::cout<<e<<" "<<f<<" "<<a<<"\n";

    return 0;
}
