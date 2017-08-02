cmake_minimum_required (VERSION 3.1)
project (playground)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.2 REQUIRED)

# put all source code in one place for convenience
file(GLOB_RECURSE physics physics/src/*.cpp physics/include/physics/*.hpp)
file(GLOB_RECURSE ctrnn ctrnn/src/*.cpp ctrnn/include/ctrnn/*.hpp)
file(GLOB_RECURSE model model/src/*.cpp model/include/model/*.hpp)
file(GLOB_RECURSE simulator simulator/src/*.cpp simulator/include/simulator/*.hpp)
file(GLOB_RECURSE neat neat/src/*.cpp neat/include/*.hpp)
file(GLOB_RECURSE graphics graphics/src/*.cpp graphics/include/graphics/*.hpp)

# ensure headers in the src folder are compiler-found
include_directories(physics/include)
include_directories(ctrnn/include)
include_directories(model/include)
include_directories(graphics/include)
include_directories(simulator/include)
include_directories(neat/include)
include_directories(graphics/include)
include_directories(/usr/local/include/)
include_directories( ${OPENGL_INCLUDE_DIRS} )

# break above sub-folders into individual libraries
add_library(physics_lib ${physics})
add_library(ctrnn_lib ${ctrnn})
add_library(model_lib ${model})
add_library(simulator_lib ${simulator})
add_library(neat_lib ${neat})
add_library(graphics_lib ${graphics})
add_executable(stest main/src/app.cpp)
target_link_libraries(stest model_lib ctrnn_lib physics_lib simulator_lib neat_lib graphics_lib ${OPENGL_LIBRARIES} glfw)

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -std=c++14 -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic -Wno-deprecated)
target_compile_options(physics_lib PUBLIC ${COMP_FLAGS})
target_compile_options(ctrnn_lib PUBLIC ${COMP_FLAGS})
target_compile_options(model_lib PUBLIC ${COMP_FLAGS})
target_compile_options(simulator_lib PUBLIC ${COMP_FLAGS})
target_compile_options(neat_lib PUBLIC ${COMP_FLAGS})
target_compile_options(graphics_lib PUBLIC ${COMP_FLAGS})
target_compile_options(stest PUBLIC ${COMP_FLAGS})
