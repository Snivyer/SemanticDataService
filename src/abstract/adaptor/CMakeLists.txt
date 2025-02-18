add_library(
    adaptor
    OBJECT
    local_adaptor.cpp
    NC_pluge.cpp
    HDF_pluge.cpp
    pluge_factory.cpp
)



set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:adaptor> PARENT_SCOPE)