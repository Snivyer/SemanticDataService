#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "manager/databox/databox_object.h"


namespace SDS
{
     #define HANDLE_SIGPIPE(s, fd_)                                                      \
     do {                                                                                \
          Status _s = (s);                                                               \
          if (!_s.ok()) {                                                                \
               if (errno == EPIPE || errno == EBADF || errno == ECONNRESET) {            \
                    ARROW_LOG(WARNING)                                                   \
                         << "Received SIGPIPE, BAD FILE DESCRIPTOR, or ECONNRESET when " \
                         "sending a message to client on fd "                            \
                         << fd_ << ". "                                                  \
                         "The client on the other end may have hung up.";                \
               } else {                                                                  \
                    return _s;                                                           \
               }                                                                         \
          }                                                                              \
     } while (0);

     typedef enum {
          
          // databox was created but is not filled with data by adaptor
          DATABOX_CREATED = 1,
          // databox is both created and filled with data
          DATABOX_FILLED = 2,
          // databox is undersend,
          DATABOX_UNSEND = 3,
     } DataBoxState;

     typedef enum {
          // The object was not found.
          OBJECT_NOT_FOUND = 0,
          // The object was found.
          OBJECT_FOUND = 1
     } object_status;

     // communicate with databox client
     struct Client {
        int fd;

        Client(int fd) {
          this->fd = fd;
        }

     };

     typedef struct {
        // The ID of the requested databox.
        ContentID cntID;
     } DataBoxRequest;


     // Mapping from DataBox IDs to request
     typedef std::unordered_map<ContentID, DataBoxRequest, ContentIDHasher> DataBoxRequestMap;


     typedef struct {
        ContentID cntID;
        std::unordered_set<Client*> clients;
        DataBoxState state;
        DataboxObject* ptr;
     } DataBoxTableEntry;


     struct DataBoxStoreInfo {

          // DataBox that are in the DataBox Store
          std::unordered_map<ContentID, DataBoxTableEntry*, ContentIDHasher> databoxs;

          // The amount of memory that we allow to be allocated in the store.
          int64_t memoryCapacity;
     };




   


}