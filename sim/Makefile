
CXX = clang++
CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++17 -O3 -I$(HOME)/others/reactphysics3d/src -Wno-unused-parameter -Wno-overloaded-virtual -Wno-nested-anon-types
BUILD_DIR = build
BIN = $(BUILD_DIR)/sim
CPP = $(wildcard *.cpp)
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
LIBS = -lm -lreactphysics3d
LDFLAGS = -L$(HOME)/local/lib

$(BIN): $(OBJ) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(OBJ) -o $(BIN) $(LIBS)

-include $(DEP)

$(BUILD_DIR)/%.o: %.cpp Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: run
run: $(BIN)
	time $(BIN)

