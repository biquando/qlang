CC = clang++
LD = clang++
CPPFLAGS := $(CPPFLAGS) --std=c++17 -Wall -Wextra -pedantic -Isrc -g -O0

SRC = $(wildcard src/*.cpp) $(wildcard src/lexer/*.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

TESTS_SRC = $(wildcard tests/test_*.cpp)
TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
TESTS_DEP = $(TESTS_SRC:.cpp=.d)
TESTS_BIN = $(TESTS_SRC:tests/test_%.cpp=tests/test-%)
TESTS = $(subst tests/,,$(TESTS_BIN))

.PHONY: all $(TESTS) clean
.PRECIOUS: $(OBJ) $(TESTS_OBJ)

all: $(TESTS_BIN)

$(TESTS): %: tests/%
	tests/$@


tests/test-%: tests/test_%.o $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

-include $(DEP)
-include $(TESTS_DEP)

%.o: %.cpp Makefile
	$(CC) -o $@ -c $< $(CPPFLAGS) -MMD -MP


clean:
	rm -rf $(OBJ) $(DEP) $(TESTS_OBJ) $(TESTS_DEP) $(TESTS_BIN)
