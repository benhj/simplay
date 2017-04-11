cmake_minimum_required (VERSION 2.6)
project (playground)

# put all source code in one place for convenience
file(GLOB_RECURSE physics physics/src/*.cpp physics/include/*.hpp)

# ensure headers in the src folder are compiler-found
include_directories(physics/include)

# break above sub-folders into individual libraries
add_library(physics_lib ${physics})

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -std=c++14 -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic)
target_compile_options(physics_lib PUBLIC ${COMP_FLAGS})
