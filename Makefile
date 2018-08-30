
CXX = c++
CXX_FLAGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -std=c++11 -O3 -ffast-math
BUILD_DIR = build
BIN = $(BUILD_DIR)/ray
CPP = $(wildcard *.cpp)
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
LIBS = -lm -lpthread
LDFLAGS =
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
	time for frame in $$(seq -f "%03g" 0 239); do /bin/echo -n "$$frame: "; $(BIN) $$frame anim/out-$$frame.png 100; done

.PHONY: gif
gif:
	convert -loop 0 -delay 3 anim/out-*.png out.gif

.PHONY: docker-build
docker-build: $(BIN)
	docker build -t ray .

.PHONY: docker-run
docker-run:
	docker run -it --name ray-run ray

.PHONY: docker-get
docker-get:
	docker cp ray-run:out.png out.png

