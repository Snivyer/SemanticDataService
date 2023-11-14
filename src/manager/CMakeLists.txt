add_library(
  manager
  OBJECT
  cnt_meta.cpp
  sto_meta.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:manager>
  PARENT_SCOPE)