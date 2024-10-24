add_subdirectory(data_rpc)

add_library(rpc STATIC ${ALL_OBJECT_FILES})

set(RPC_LIBS
    data_rpc
)

target_link_libraries(rpc ${RPC_LIBS} )
target_include_directories(
    rpc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)










