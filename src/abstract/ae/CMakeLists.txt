add_library(
    ae
    OBJECT
    ae.c
    # ae_epoll.c
    # ae_evport.c
    # ae_kqueue.c
    # ae_select.c
)



set(ALL_OBJECT_FILES ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ae> PARENT_SCOPE)