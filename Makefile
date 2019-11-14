CC=/usr/local/ge/opt/intel-2019u3/compilers_and_libraries_2019.3.199/linux/bin/intel64/icpc
ICPC_FLAGS=-gxx-name=/opt/rh/devtoolset-7/root/usr/bin/g++ -gcc-name=/opt/rh/devtoolset-7/root/usr/bin/gcc
INC=-I/usr/local/ge/opt/igThirdPartyLibs/include -Iinclude
LIB=-L/usr/local/ge/opt/igThirdPartyLibs/lib -lyaml-cpp
CC_FLAGS=-std=c++17 -O3 -Wall -Werror -pedantic
TEST_DIR=src/tests
BUILD_DIR=build

all: tests

tests: $(BUILD_DIR)/tests $(BUILD_DIR)/tests/test $(BUILD_DIR)/tests/cmd_yaml_test $(BUILD_DIR)/tests/yaml_test

$(BUILD_DIR)/tests:
	mkdir -p $@

$(BUILD_DIR)/tests/%: $(TEST_DIR)/$(basename $(notdir %)).cpp
	$(CC) $(ICPC_FLAGS) $(CC_FLAGS) $(INC) $(LIB) $(CC_FLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
