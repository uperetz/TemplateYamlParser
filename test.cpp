#include<sstream>
#include<iostream>
#include<any>
#include"argument.h"
#include"parser.h"
#include<stdint.h>
#include<string>
#include<vector>

int main() {
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
    ArgList argm(req, opt, 5, {std::string("hi"), 21, 'a'});

    Booleans bla{"abc", "def"};
    Booleans bl(bla);

    argm.set("int", "3");
    argm.set("uint32_t", "123");
    argm.set("string", "yay");
    
    argm.set(0, "321");
    argm.set(1, "1");

    argm.collect(w, x, z, y, a, b, c, d, e);
    std::cout<<x<<" : "<<y<<" : "<<z<<" : "<<w<<" : "<<a<<" : "<<b<<" : "<<c<<" : "<<d<<" : "<<e<<"\n";

    std::cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";

    Parser p = Parser( RequiredNames{"int", "string", "uint32_t"},
                       OptionalNames{ {"std::string", std::string("Hello world")} },
                       Booleans{"bool"}, 3);//, VecOptional{std::string("astring")});

    std::string f;

    p.parse_arguments({"a", "-bool", "-uint32_t", "23", "-int", "213", "mc", "-string", "aba"});
    p.collect(b, x, z, c, y, e, a, f);
    std::cout<<b<<" "<<x<<" "<<z<<" "<<c<<" "<<y<<" "<<e<<" "<<a<<" "<<f<<"\n";

    return 0;
}
