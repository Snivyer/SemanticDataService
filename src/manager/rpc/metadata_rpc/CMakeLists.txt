add_library(
    metadata_rpc
    OBJECT
    protocol.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:data_rpc> PARENT_SCOPE)