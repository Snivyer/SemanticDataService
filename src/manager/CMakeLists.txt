add_library(
  manager
  OBJECT
  content_metadata.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:manager>
  PARENT_SCOPE)