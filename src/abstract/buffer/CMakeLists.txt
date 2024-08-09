add_subdirectory(base)
add_subdirectory(malloc)
add_subdirectory(memory)

add_library(buffer STATIC ${ALL_OBJECT_FILES})

set(BUFFER_LIBS
    base
    malloc
    memory
)


target_link_libraries(buffer ${BUFFER_LIBS} )

target_include_directories(
    buffer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)










