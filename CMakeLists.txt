# Copyright 2023 The Sematic Data Services Authors. All rights reserved.

cmake_minimum_required(VERSION 3.26.3)
project(SDS VERSION 1.0 )



add_compile_options(-std=c++11)

set(SDS_SRC_INCLUDE_DIR 
        ${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/src/include/manager
    )


include_directories(${SDS_SRC_INCLUDE_DIR})

add_subdirectory(src)  


add_executable(SDS_test main.cpp)
target_link_libraries(SDS_test SDS_lib)