add_library(
    index_pb
    OBJECT
    index.pb.cc
    index.grpc.pb.cc
)


set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:index_pb> PARENT_SCOPE)