add_subdirectory(dataspace)
add_subdirectory(storagespace)
add_subdirectory(semanticspace)


add_library(API STATIC ${ALL_OBJECT_FILES})

# set(API_LIBS
#         dataspace
#         storagespace
#         semanticspace
#         ) 

# target_link_libraries(
#         API
#         ${ABSTRACT_LIBS}
#         )

target_include_directories(
        API PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)