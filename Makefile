
CXX = clang++
CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++17 -O3 -ffast-math
BUILD_DIR = build
BIN = $(BUILD_DIR)/ray
CPP = $(wildcard *.cpp)
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
IMG = out.ppm

$(BIN): $(OBJ) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $(BIN) -lm

-include $(DEP)

$(BUILD_DIR)/%.o: %.cpp Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

$(IMG): $(BIN)
	$(BIN) > $(IMG) && touch $(IMG)
