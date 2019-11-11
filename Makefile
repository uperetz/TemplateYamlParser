CC="/usr/local/ge/opt/intel-2019u3/compilers_and_libraries_2019.3.199/linux/bin/intel64/icpc"
all:
	$(CC) -std=c++17 -O3 -Wall -Werror -pedantic test.cpp -o test
