add_library(
    data_rpc
    OBJECT
    server.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:data_rpc> PARENT_SCOPE)