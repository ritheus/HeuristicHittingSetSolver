cmake_minimum_required(VERSION 3.10)
project(hittingset)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(hittingset ${SOURCES})

target_include_directories(hittingset PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_include_directories(hittingset PRIVATE /usr/include)
target_link_directories(hittingset PRIVATE /usr/lib/x86_64-linux-gnu)
target_link_libraries(hittingset PRIVATE glpk)