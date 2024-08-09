add_library(
    base
    OBJECT
    buffer.cpp
    bufferV.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:base> PARENT_SCOPE)