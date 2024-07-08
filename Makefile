CC = clang++
CPPFLAGS := $(CPPFLAGS) --std=c++17 -Wall -Wextra -pedantic

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

.PHONY: run clean

bin/test: $(OBJ)
	mkdir -p bin
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(DEP)

%.o: %.cpp Makefile
	$(CC) -o $@ -c $< $(CPPFLAGS) -MMD -MP

run: bin/test
	bin/test

clean:
	rm -rf bin $(OBJ) $(DEP)
