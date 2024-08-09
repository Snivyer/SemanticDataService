add_library(
  dataspace
  OBJECT
  data_box.cpp
  data_management.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:dataspace> PARENT_SCOPE)