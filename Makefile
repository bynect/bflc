CXX=g++
CPPFLAGS=-g3 -fsanitize=address -O2 -Wall -Wextra -Werror

SRC=$(shell find src -name '*.cpp')
OBJ=$(SRC:.cpp=.o)
OUT=bfc

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(CPPFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(OUT)
