CC="/opt/rh/devtoolset-7/root/usr/bin/g++"
all:
	$(CC) -std=c++17 -O3 -Wall -Werror -pedantic parser.hpp argument.hpp test.cpp -o test
