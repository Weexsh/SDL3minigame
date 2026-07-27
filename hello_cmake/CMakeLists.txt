cmake_minimum_required(VERSION 3.12)
project(SDL3_VSCode_Demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("SDL3/include")

add_executable(miniGame miniGame.cpp)

target_link_libraries(miniGame "${PROJECT_SOURCE_DIR}/SDL3/lib/libSDL3.dll.a")