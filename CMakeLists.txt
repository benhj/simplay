cmake_minimum_required (VERSION 2.6)
project (playground)

# put all source code in one place for convenience
file(GLOB_RECURSE physics physics/src/*.cpp physics/include/*.hpp)
file(GLOB_RECURSE ctrnn ctrnn/src/*.cpp ctrnn/include/*.hpp)
file(GLOB_RECURSE model model/src/*.cpp model/include/*.hpp)

# ensure headers in the src folder are compiler-found
include_directories(physics/include)
include_directories(ctrnn/include)
include_directories(model/include)

# break above sub-folders into individual libraries
add_library(physics_lib ${physics})
add_library(ctrnn_lib ${ctrnn})
add_library(model_lib ${model})

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -std=c++14 -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic)
target_compile_options(physics_lib PUBLIC ${COMP_FLAGS})
target_compile_options(ctrnn_lib PUBLIC ${COMP_FLAGS})
target_compile_options(model_lib PUBLIC ${COMP_FLAGS})
