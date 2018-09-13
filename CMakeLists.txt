cmake_minimum_required (VERSION 3.5)
project (Weekend_ray_tracer)

# We need protobuf.
find_package(Protobuf REQUIRED)

# Normal flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -Wall -Wextra -Wpedantic -Wshadow -O3 -ffast-math")

# Unfortunately protobuf's generated C++ code is pretty crappy, so we have to turn
# off a bunch of warnings globally.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nested-anon-types -Wno-unused-parameter -Wno-deprecated-declarations -Wno-sign-compare")

# Our source files.
file(GLOB SOURCES "src/*.cpp")

# Generate protobuf files.
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS Scene.proto)
set(SOURCES ${SOURCES} ${PROTO_SRCS})

# Our binary.
add_executable(ray ${SOURCES})

# What to link with.
target_link_libraries(ray m pthread ${PROTOBUF_LIBRARIES})

# We need these C++ features.
target_compile_features(ray PRIVATE cxx_long_long_type cxx_thread_local)

# Look for the standard protobuf headers, as well as the one we generated.
target_include_directories(ray PRIVATE ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# If we're on MacOS, add minifb.
if (APPLE)
    message ("-- Adding minifb to display rendered images")

    # Add the library directory.
    add_subdirectory(src/minifb)

    # Tell our code to use it.
    target_compile_definitions(ray PRIVATE DISPLAY)
    target_include_directories(ray PRIVATE ${minifb_SOURCE_DIR})

    # Need the Cocoa framework.
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(ray minifb ${COCOA_LIBRARY})
endif()
