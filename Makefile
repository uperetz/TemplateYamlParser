export

ROOT=$(realpath .)
CC=/usr/local/ge/opt/intel-2019u3/compilers_and_libraries_2019.3.199/linux/bin/intel64/icpc
INC=-I/usr/local/ge/opt/igThirdPartyLibs/include -I$(ROOT)/include
LIB=-L/usr/local/ge/opt/igThirdPartyLibs/lib -lyaml-cpp
CC_FLAGS=-std=c++17 -O3 -Wall -Werror -pedantic
TEST_DIR=testing/src
BUILD_DIR=$(ROOT)/build

all: test

test:
	+$(MAKE) -C src $(MAKECMDGOALS)
