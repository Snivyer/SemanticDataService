add_subdirectory(utils)
add_subdirectory(GIS)
add_subdirectory(meta)
add_subdirectory(buffer)
add_subdirectory(adaptor)


add_library(abstract STATIC ${ALL_OBJECT_FILES})

set(ABSTRACT_LIBS
        utils
        GIS
        meta
        buffer
        adaptor
        ) 


target_link_libraries(
        abstract
        ${ABSTRACT_LIBS}
        )


target_include_directories(
        abstract PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)