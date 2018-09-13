cmake_minimum_required (VERSION 3.5)
project (Weekend_ray_tracer)

add_subdirectory($ENV{HOME}/others/reactphysics3d reactphysics3d)
add_subdirectory(src/scene)
add_subdirectory(src/ray)
add_subdirectory(src/sim)

