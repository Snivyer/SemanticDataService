add_library(
    index
    OBJECT
    space_index.cpp
    time_index.cpp
    var_index.cpp
)


set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:index> PARENT_SCOPE)