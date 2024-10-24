add_subdirectory(metadata)
add_subdirectory(index)
add_subdirectory(databox)



add_library(manager STATIC ${ALL_OBJECT_FILES})

set(MANAGER_LIBS
        metadata
        index
        databox
        ) 


target_link_libraries(
        manager
        ${MANAGER_LIBS}
        )


target_include_directories(
        manager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)