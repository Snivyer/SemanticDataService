add_library(
  semanticspace
  OBJECT
  semantic_space.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:semanticspace> PARENT_SCOPE)