add_library(
    event
    OBJECT
    events.cpp
)



set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:event> PARENT_SCOPE)