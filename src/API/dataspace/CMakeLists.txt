add_library(
  dataspace
  OBJECT
  data_management.cpp
  data_box.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:dataspace> PARENT_SCOPE)