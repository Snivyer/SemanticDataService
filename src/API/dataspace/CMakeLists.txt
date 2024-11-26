add_library(
  dataspace
  OBJECT
  databox_store.cpp
  databox_client.cpp
  meta_service.cpp
  meta_service_client.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:dataspace> PARENT_SCOPE)