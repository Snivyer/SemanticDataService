 #pragma once

#include <memory>
#include <vector>

#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/event/events.h"
#include "abstract/IO/io.h"
#include "abstract/adaptor/adaptor.h"
#include "manager/databox/databox_manager.h"
#include "manager/databox/databox_object.h"



namespace SDS
{


    struct GetRequest {
        // the client that called get
        Client* client;
        // The ID of the timer that will time out and cause this wait to return 
        int64_t timer;
        // The databox ID involved in this request.
        std::vector<ContentID> databoxIDs;
        // The object information for the objects in this request. This is used in the reply.
        std::unordered_map<ContentID, DataboxObject, ContentIDHasher> databoxs;

        GetRequest(Client *client, const std::vector<ContentID> &ids) {
            this->client = client;
            this->timer = -1;

            for(auto cntID : ids) {
                this->databoxIDs.push_back(cntID);
            }
        }
    };




    class DataBoxStore {
        public:
            ~DataBoxStore();

            static std::shared_ptr<DataBoxStore> createStore(std::shared_ptr<EventLoop> loop,
                                                             int64_t systemMemory);

            // create a databox object and add it into the dbentry
            bool createDB(const ContentID& cntID, FilePathList &filePath,
                            std::shared_ptr<Client> client, std::shared_ptr<DataboxObject>);

            
            // delete db objects that have been created
            bool deleteDB(const std::vector<ContentID> &ids);
            

            // get db object
            bool getDB(std::shared_ptr<Client> client, const std::vector<ContentID> &ids,
                     int64_t timeout_ms);
 
            // seal an databox, this databox is now immutable and can be accessed with get.
            bool sealDB(const ContentID &cntID);

            // check if the databox store contains an databox
            bool containDB(const ContentID &cntID);

            // release a client that is no longer using an object
            bool releaseDB(const ContentID &cntID, std::shared_ptr<Client> client);

            // connect a new client to the databox Store
            void connectClient(int listenerSock); 

            // Disconnect a client from the databox Store
            void disconnectClient(Client* client);

            // the main message process 
            arrow::Status processMessage(Client* client);



    
        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxStore(std::shared_ptr<Impl> impl);
            bool addClientToEntry(DataBoxTableEntry* entry, std::shared_ptr<Client> client);
            bool removeClientFromEntry(DataBoxTableEntry* entry, std::shared_ptr<Client> client);
            
            
            
            DataBoxTableEntry* getDBObjectEntry(const ContentID &cntID);
                
                

    };

   


}