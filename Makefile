
CXX = clang++
CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++17 -O3 -ffast-math
BUILD_DIR = build
BIN = $(BUILD_DIR)/ray
CPP = $(wildcard *.cpp)
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
LIBS = -lm
LDFLAGS =

# Comment out this section if you don't want minifb for the Mac:
CXX_FLAGS += -DDISPLAY -Iminifb
LIBS += -lminifb -framework Cocoa
LDFLAGS += -Lminifb/build

$(BIN): $(OBJ) minifb/build/libminifb.a Makefile
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
