add_subdirectory(utils)
add_subdirectory(manager)
add_subdirectory(abstract)
add_subdirectory(API)


add_library(SDS_lib STATIC ${ALL_OBJECT_FILES})

set(SDS_LIBS
        utils
        abstract
        manager
        API
) 


target_link_libraries(
        SDS_lib
        ${SDS_LIBS}
        )


target_include_directories(
        SDS_lib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )


