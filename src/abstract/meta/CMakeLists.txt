add_library(
    meta
    OBJECT
    meta_store.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:meta> PARENT_SCOPE)