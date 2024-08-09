add_library(
    malloc
    OBJECT
    STL_buffer.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:malloc> PARENT_SCOPE)