
# Which C++ compiler to use. This works for both Clang and G++.
CXX = clang++

# Location of protobuf include files, library, and compiler.
PROTOBUF_DIR = /opt/local

CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++17 -O3 -I$(HOME)/others/reactphysics3d/src -Wno-unused-parameter -Wno-overloaded-virtual -Wno-nested-anon-types -I.. -I$(PROTOBUF_DIR)/include

# Unfortunately protobuf's generated C++ code is pretty crappy, so we have to turn
# off a bunch of warnings globally.
CXX_FLAGS += -Wno-nested-anon-types -Wno-unused-parameter -Wno-deprecated-declarations -Wno-sign-compare

BUILD_DIR = build
BIN = $(BUILD_DIR)/sim
CPP = $(wildcard *.cpp)
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
LIBS = -lm -lreactphysics3d -lprotobuf
LDFLAGS = -L$(HOME)/local/lib -L$(PROTOBUF_DIR)/lib
SCENE = ../out.scene

$(BIN): $(OBJ) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(OBJ) -o $(BIN) $(LIBS)

-include $(DEP)

$(BUILD_DIR)/%.o: %.cpp Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

$(SCENE): $(BIN)
	$(BIN) $(SCENE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: run
run: $(SCENE)

