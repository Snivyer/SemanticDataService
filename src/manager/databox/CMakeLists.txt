add_library(
    databox
    OBJECT
    databox_object.cpp
    databox_store.cpp
    databox_client.cpp
)


set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:databox> PARENT_SCOPE)