add_library(
    GIS
    OBJECT
    geo_read.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:GIS> PARENT_SCOPE)