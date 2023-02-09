cmake_minimum_required(VERSION 3.19)
project(phpLite)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)
include_directories(src)
add_executable(
    phpLite src/settings.h src/settings.cpp src/utils.cpp src/utils.h src/json.h src/json.c
    src/mongoose.c src/mongoose.h src/mongoose_server.h src/mongoose_server.cpp
        src/json.c  src/main.cpp)
target_link_libraries(phpLite ${CMAKE_DL_LIBS})