add_library(
    adaptor
    OBJECT
    local_adaptor.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:adaptor> PARENT_SCOPE)