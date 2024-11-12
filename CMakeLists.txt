# Copyright 2023 The Sematic Data Services Authors. All rights reserved.

cmake_minimum_required(VERSION 3.16.3)
project(SDS VERSION 1.0 )



set(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb") # 定义Debug编译参数
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数

add_compile_options(-std=c++17)
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

find_package(Arrow REQUIRED)
find_package(ArrowFlight REQUIRED)
find_package(Threads REQUIRED)


set(SDS_SRC_INCLUDE_DIR 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/src/include/manager
    )

set(SDS_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party/ae/
)

set(SDS_THIRD_PARTY_LIB_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/third_party_lib/duckdb/include
)

include_directories(${SDS_SRC_INCLUDE_DIR} ${SDS_THIRD_PARTY_INCLUDE_DIR} ${SDS_THIRD_PARTY_LIB_INCLUDE_DIR})

add_subdirectory(src)  
add_subdirectory(third_party) 


link_directories(${PROJECT_SOURCE_DIR}/third_party_lib/duckdb/lib)


# # databox test 
add_executable(SDS_databox_test databox_test.cpp)
target_link_libraries(SDS_databox_test manager)
target_link_libraries(SDS_databox_test abstract)
target_link_libraries(SDS_databox_test netcdf)
target_link_libraries(SDS_databox_test Arrow::arrow_shared)
target_link_libraries(SDS_databox_test stdc++fs)

# # duckdb test
# add_executable(SDS_duckdb_test duckdb_test.cpp)
# target_link_libraries(SDS_duckdb_test manager)
# target_link_libraries(SDS_duckdb_test abstract)
# target_link_libraries(SDS_duckdb_test Arrow::arrow_shared)
# target_link_libraries(SDS_duckdb_test netcdf)
# target_link_libraries(SDS_duckdb_test duckdb)
# target_link_libraries(SDS_duckdb_test stdc++fs)


## arrow test
# add_executable(ARROW_test arrow_test.cpp)
# target_link_libraries(ARROW_test PRIVATE Arrow::arrow_shared)

## DB Store test
add_executable(SDS_db_store_test DB_store.cpp)
target_link_libraries(SDS_db_store_test manager)
target_link_libraries(SDS_db_store_test abstract)
target_link_libraries(SDS_db_store_test Arrow::arrow_shared)
target_link_libraries(SDS_db_store_test netcdf)
target_link_libraries(SDS_db_store_test duckdb)
target_link_libraries(SDS_db_store_test stdc++fs)
target_link_libraries(SDS_db_store_test ArrowFlight::arrow_flight_shared)
target_link_libraries(SDS_db_store_test ${CMAKE_THREAD_LIBS_INIT})

## DB store client test
add_executable(SDS_db_store_client_test DB_store_client.cpp)
target_link_libraries(SDS_db_store_client_test manager)
target_link_libraries(SDS_db_store_client_test abstract)
target_link_libraries(SDS_db_store_client_test Arrow::arrow_shared)
target_link_libraries(SDS_db_store_client_test netcdf)
target_link_libraries(SDS_db_store_client_test duckdb)
target_link_libraries(SDS_db_store_client_test stdc++fs)
target_link_libraries(SDS_db_store_client_test ArrowFlight::arrow_flight_shared)
target_link_libraries(SDS_db_store_client_test ${CMAKE_THREAD_LIBS_INIT})



# dataManager test 
# add_executable(SDS_dataManager_test dataManager_test.cpp)
# target_link_libraries(SDS_dataManager_test API)
# target_link_libraries(SDS_dataManager_test manager)
# target_link_libraries(SDS_dataManager_test abstract)
# target_link_libraries(SDS_dataManager_test netcdf)
# target_link_libraries(SDS_dataManager_test duckdb)
