CC=/usr/local/ge/opt/intel-2019u3/compilers_and_libraries_2019.3.199/linux/bin/intel64/icpc
INC=-I/usr/local/ge/opt/igThirdPartyLibs/include
LIB=-L/usr/local/ge/opt/igThirdPartyLibs/lib -lyaml-cpp
CC_FLAGS=-std=c++17 -O3 -Wall -Werror -pedantic

all: yaml test

test:
	$(CC) $(INC) $(LIB) $(CC_FLAGS) test.cpp -o test
yaml:
	$(CC) $(INC) $(LIB) $(CC_FLAGS) yaml_test.cpp -o yaml_test
