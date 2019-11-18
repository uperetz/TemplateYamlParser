#include<iostream>
#include"yaml-parser/parser.h"

void f(std::string s, long x) {
    std::cout<<"Running f("<<s<<", "<<x<<")\n";
}

void f1(char s, long x) {
    std::cout<<"Running f("<<s<<", "<<x<<")\n";
}

int main(int argc, char **argv) {
    CMD::Scalar val;
    std::cout<<"Scalar stuff:\n";
    std::cout<<val;
    
    std::cout<<"++++++++++++++++++++++++++++++++++++++++++\n";
    CMD::Sequence s(
        CMD::Dependencies(),
        CMD::Scalar{}, val, CMD::Scalar{}
    );

    // val.set<int>("3.2"); // break
    val.set<float>("3.2");
    float bla;
    std::cout<<val.get<float>(bla)<<"\n";
    std::cout<<"\nSequence:\n";
    std::cout<<s;
    std::cout<<"++++++++++++++++++++++++++++++++++++++++++\n";
    CMD::Map m1(
        CMD::Dependencies(),
        "LONG", CMD::Scalar{(long)122}, // will assume int without cast - on you
        "CHAR", CMD::Scalar("a")
    );
    //This is how you would debug - don't use (unless you really want to?)
    //dynamic_cast<CMD::Scalar*>(&m1["LONG"])->set<long>("234");
    //long bla1;
    //std::cout<<dynamic_cast<CMD::Scalar*>(&m1["LONG"])->get<long>(bla1)<<"\n";
    std::cout<<m1;
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";

    CMD::Map m(
        CMD::Dependencies{"0.hello.1"},
        "first", val,
        "another", s,
        "string", CMD::Scalar("abc!"),
        "third",
            CMD::Sequence(
                CMD::Dependencies(),
                CMD::Scalar{},
                m1,
                CMD::Scalar("1")
            ),
        "val again", val
    );

    std::cout<<"Map :\n";
    std::cout<<m;
    
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";
    CMD::ArgPath<float> t;
    // short x; // break
    float x;
    t.parse_yaml(val, x);
    std::cout<<x<<"\n";
    x = 1;
    CMD::ArgPath<float> t1("first");
    t1.parse_yaml(m, x);
    std::cout<<x<<"\n";
    CMD::ArgPath<long> t2("third", 1, "LONG"); // using long->short will break
    CMD::ArgPath<std::string> t3("third", 2); // 
    CMD::ArgPath<std::string> t4("third", 1, "CHAR"); // using std::string->char will break
    // short y; break (combine with long-> short above)
    long y;
    //This is how you would debug (not meant to be used)
    //std::cout<< 
    //    dynamic_cast<CMD::Scalar*>(
    //        (& (*dynamic_cast<CMD::Map*>(
    //            &( (*dynamic_cast<CMD::Sequence*>(&m["third"]))[1] )
    //            ))["LONG"]) 
    //                )->get<long>(bla1)<<"\n";
    t2.parse_yaml(m, y);
    std::cout<<y<<" "<<t3.parse_yaml(m)<<" "<<t4.parse_yaml(m)<<"\n";
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";
    CMD::Parser p(m);
    std::cout<<"FINAL\n";
    p.parse_yaml("/home/de781561/code/parser/igets_cases/case1.yml");
    p.apply(f, t4, t2);
    CMD::ArgPack a1(f, t4, t2);
    p.apply(a1);
    // Giving a function incompatible with one of the paths is a massive compiler error.
    p.apply_one(CMD::ArgPack(f1,CMD::ArgPath<char>("third", 1, "CHAR"), t2),
                //CMD::ArgPack(f, t1, t2)); can't use - type mismatch t1 and f!
                //CMD::ArgPack(f, CMD::ArgPath<std::string>("thirdd"), t2)); Fail all
                CMD::ArgPack(f, CMD::ArgPath<std::string>("third", 1, "CHAR"), t2));
    std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++\n";

    std::cout<<"System test:\n\n";
    CMD::Parser parser(
        CMD::Map(
            CMD::Dependencies{},
            "LIST", CMD::Sequence(
                CMD::Dependencies{},
                CMD::Scalar("inner"),
                CMD::Sequence(
                    CMD::Dependencies{},
                    CMD::Scalar{2}, CMD::Scalar{}, CMD::Scalar((short)0),
                    CMD::Map(
                        CMD::Dependencies{},
                        "lonely", CMD::Scalar("sea")
                    )
                )
            ),
            "LONG", CMD::Scalar{(long)122}, // will assume int without cast - on you
            "CHAR", CMD::Scalar('a')
        )
    );
    
    auto l1 = [](int x, char y, const std::string& z) {
        std::cout<<"Ran l1("<<x<<","<<y<<","<<z<<")\n";};

    parser.parse_yaml("/home/de781561/code/parser/igets_cases/case2.yml");
    parser.apply_one(
        CMD::ArgPack(l1,
            CMD::ArgPath<int>("LIST", 1, 3),
            CMD::ArgPath<char>("CHAR"),
            CMD::ArgPath<std::string>("LIST", 0)
        ),
        CMD::ArgPack(l1,
            CMD::ArgPath<int>("LIST", 1, 0),
            CMD::ArgPath<char>("CHAR"),
            CMD::ArgPath<std::string>("LIST", 0)
        )
    );
    return 0;
}
