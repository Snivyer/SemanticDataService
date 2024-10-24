add_subdirectory(utils)
add_subdirectory(GIS)
add_subdirectory(meta)
add_subdirectory(adaptor)
add_subdirectory(rpc)
add_subdirectory(event)
add_subdirectory(IO)


add_library(abstract STATIC ${ALL_OBJECT_FILES})

set(ABSTRACT_LIBS
        utils
        GIS
        meta
        adaptor
        rpc
        event
        ) 


target_link_libraries(
        abstract
        ${ABSTRACT_LIBS}
        )


target_include_directories(
        abstract PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)