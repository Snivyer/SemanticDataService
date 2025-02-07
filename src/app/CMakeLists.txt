add_library(
    retrieval
    OBJECT
    SDS_retrieval.cpp
)

set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS: retrieval> PARENT_SCOPE)