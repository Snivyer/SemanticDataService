add_library(
    metadata
    OBJECT
    cnt_meta.cpp
    sto_meta.cpp
)


set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:metadata> PARENT_SCOPE)