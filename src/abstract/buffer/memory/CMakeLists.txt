add_library(
    memory
    OBJECT
    memory.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:memory> PARENT_SCOPE)