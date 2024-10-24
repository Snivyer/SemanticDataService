#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/event/events.h"
#include "manager/databox/databox_object.h"


namespace SDS
{

     typedef enum {
          // databox was created but not sealed in the store
          DATABOX_CREATED = 1,
          // databox is sealed and stored in the store
          DATABOX_SEALED,
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
          fd = fd;
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
        std::unordered_set<std::shared_ptr<Client>> clients;
        DataBoxState state;
        std::shared_ptr<DataboxObject> ptr;
     } DataBoxTableEntry;


     struct DataBoxStoreInfo {

          // DataBox that are in the DataBox Store
          std::unordered_map<ContentID, std::unique_ptr<DataBoxTableEntry>, ContentIDHasher> databoxs;

          // The amount of memory that we allow to be allocated in the store.
          int64_t memoryCapacity;
     };




   


}