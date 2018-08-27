
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

.PHONY: anim
anim: $(BIN) Makefile
	if [ -d anim ]; then rm -r anim; fi
	mkdir -p anim
	time for frame in $$(seq -f "%03g" 0 59); do /bin/echo -n "$$frame: "; $(BIN) $$frame anim/out-$$frame.png 100; done

.PHONY: gif
gif:
	convert -loop 0 -delay 3 anim/out-*.png out.gif
