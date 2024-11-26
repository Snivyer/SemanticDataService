add_library(
  utils
  OBJECT
  string_operation.cpp
  directory_operation.cpp
  time_operation.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:utils> PARENT_SCOPE)