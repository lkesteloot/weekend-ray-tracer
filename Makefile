
# Which C++ compiler to use. This works for both Clang and G++.
CXX = c++

# Location of protobuf include files, library, and compiler.
PROTOBUF_DIR = /opt/local

CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++11 -O3 -ffast-math -I$(PROTOBUF_DIR)/include

# Unfortunately protobuf's generated C++ code is pretty crappy, so we have to turn
# off a bunch of warnings globally.
CXX_FLAGS += -Wno-nested-anon-types -Wno-unused-parameter -Wno-deprecated-declarations -Wno-sign-compare

# We stick everything in this directory. Easier to ignore (in git) and clean up.
BUILD_DIR = build

# The binary we build.
BIN = $(BUILD_DIR)/ray

# Collect all the source files we compile.
PB_CPP = Scene.pb.cpp
CPP = $(wildcard *.cpp)
ifeq (,$(findstring $(PB_CPP),$(CPP)))
	# Add manually if we've never built it.
	CPP += $(PB_CPP)
endif
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
LIBS = -lm -lpthread -lprotobuf
LDFLAGS = -L$(PROTOBUF_DIR)/lib
BIN_DEPS =

# If you've already built minifb, we'll use it.
ifneq (,$(wildcard minifb/build/libminifb.a))
	CXX_FLAGS += -DDISPLAY -Iminifb
	LIBS += -lminifb -framework Cocoa
	LDFLAGS += -Lminifb/build
	BIN_DEPS += minifb/build/libminifb.a
endif

$(BIN): $(OBJ) $(BIN_DEPS) Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(OBJ) -o $(BIN) $(LIBS)

Scene.pb.cpp: Scene.proto
	$(PROTOBUF_DIR)/bin/protoc --cpp_out=. Scene.proto
	mv Scene.pb.cc Scene.pb.cpp

-include $(DEP)

$(BUILD_DIR)/%.o: %.cpp Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: run
run: $(BIN)
	time $(BIN) out.scene

.PHONY: anim
anim: $(BIN) Makefile
	if [ -d anim ]; then rm -r anim; fi
	mkdir -p anim
	time $(BIN) 0,19 anim/out 1000

.PHONY: gif
gif:
	convert -loop 0 -delay 3 anim/out-*.png out.gif

.PHONY: mp4
mp4:
	ffmpeg -pattern_type glob -i 'anim/*.png' -pix_fmt yuv420p -y out.mp4

.PHONY: docker-build
docker-build: $(BIN)
	docker build -t ray .

.PHONY: docker-run
docker-run:
	docker run -it --name ray-run ray

.PHONY: docker-get
docker-get:
	docker cp ray-run:out.png out.png

