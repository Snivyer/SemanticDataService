add_library(
  manager
  OBJECT
  cnt_meta.cpp
  space_index.cpp
  sto_meta.cpp
  time_index.cpp
  var_index.cpp
)


set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:manager> PARENT_SCOPE)