
CXX = clang
CXX_FLAGS = -Wpedantic -Werror -Wall -O3
BUILD_DIR = build
LIB = $(BUILD_DIR)/libminifb.a
M = $(wildcard *.m)
OBJ = $(M:%.m=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)

$(LIB): $(OBJ) Makefile
	mkdir -p $(@D)
	$(AR) -r $(LIB) $(OBJ)

-include $(DEP)

$(BUILD_DIR)/%.o: %.m Makefile
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

