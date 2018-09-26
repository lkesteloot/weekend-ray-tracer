cmake_minimum_required(VERSION 3.5)
project(Weekend_ray_tracer)

# User-settable options.
option(BUILD_SIM "build the physics simulator" OFF)
set(RP3D_DIR $ENV{HOME}/others/reactphysics3d CACHE FILEPATH "location of reactphysics3d" )

add_subdirectory(src/scene)

if(BUILD_SIM)
    add_subdirectory(${RP3D_DIR} reactphysics3d)
    add_subdirectory(src/sim)
endif()

add_subdirectory(src/ray)

