add_library(
  storagespace
  OBJECT
  storage_space.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:storagespace> PARENT_SCOPE)