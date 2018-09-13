cmake_minimum_required (VERSION 3.5)
project (sim)

# We need protobuf.
find_package(Protobuf REQUIRED)

# Normal flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -O3 -ffast-math")

# Unfortunately protobuf's generated C++ code is pretty crappy, so we have to turn
# off a bunch of warnings globally.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nested-anon-types -Wno-unused-parameter -Wno-deprecated-declarations -Wno-sign-compare")

# Our source files.
file(GLOB SOURCES "*.cpp")

# Our binary.
add_executable(sim ${SOURCES})

# What to link with.
target_link_libraries(sim m scene reactphysics3d ${PROTOBUF_LIBRARIES})

# We need these C++ features.
target_compile_features(sim PRIVATE cxx_long_long_type cxx_thread_local)

# Look for the standard protobuf headers, as well as the one we generated.
target_include_directories(sim PRIVATE ${PROTOBUF_INCLUDE_DIRS} ${scene_BINARY_DIR} ${REACTPHYSICS3D_SOURCE_DIR}/src)

