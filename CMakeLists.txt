cmake_minimum_required(VERSION 3.20)
project(TestGL)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Debug)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    # Print a message to the user to indicate that we are using ccache.
    message(STATUS "Using ccache.")
endif(CCACHE_FOUND)

# The additional / is important to remove the last character from the path.
# Note that it does not matter if the OS uses / or \, because we are only
# saving the path size.
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# Define the NDEBUG macro to disable assertions in release builds.
#add_definitions("-DNDEBUG")

# Add gmon.out
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")


file(GLOB_RECURSE cpp_sources CONFIGURE_DEPENDS "src/common/*.cpp")
file(GLOB_RECURSE c_sources CONFIGURE_DEPENDS "src/common/*.c")

set(sources ${cpp_sources} ${c_sources} "src/main.cpp")

include_directories(includes)

add_executable(TestGL ${sources})

target_link_libraries(TestGL glfw)
target_link_libraries(TestGL GL)
