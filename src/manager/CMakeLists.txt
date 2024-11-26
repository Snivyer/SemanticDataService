add_subdirectory(metadata)
add_subdirectory(index)
add_subdirectory(databox)
add_subdirectory(rpc)
add_subdirectory(proto)
add_subdirectory(storagespace)
add_subdirectory(semanticspace)



add_library(manager STATIC ${ALL_OBJECT_FILES})

set(MANAGER_LIBS
        metadata
        index
        databox
        index_pb
        storagespace
        semanticspace
        ) 


target_link_libraries(
        manager
        ${MANAGER_LIBS}
        )


target_include_directories(
        manager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)