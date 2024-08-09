# Copyright 2023 The Sematic Data Services Authors. All rights reserved.

cmake_minimum_required(VERSION 3.20.0)
project(SDS VERSION 1.0 )


set(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数

add_compile_options(-std=c++11)


set(SDS_SRC_INCLUDE_DIR 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/src/include/manager
    )

set(SDS_THIRD_PARTY_INCLUDE_DIR
)

set(SDS_THIRD_PARTY_LIB_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/third_party_lib/duckdb/include
    ${PROJECT_SOURCE_DIR}/third_party_lib/netcdf/include
)

include_directories(${SDS_SRC_INCLUDE_DIR} ${SDS_THIRD_PARTY_INCLUDE_DIR} ${SDS_THIRD_PARTY_LIB_INCLUDE_DIR})

add_subdirectory(src)  
add_subdirectory(third_party) 


link_directories( ${PROJECT_SOURCE_DIR}/third_party_lib/duckdb/lib)
link_directories(${PROJECT_SOURCE_DIR}/third_party_lib/netcdf/lib)


# duckdb test
add_executable(SDS_duckdb_test duckdb_test.cpp)
target_link_libraries(SDS_duckdb_test SDS_lib)
target_link_libraries(SDS_duckdb_test duckdb)

# databox test 
add_executable(SDS_databox_test databox_test.cpp)
target_link_libraries(SDS_databox_test SDS_lib)
target_link_libraries(SDS_databox_test netcdf)

