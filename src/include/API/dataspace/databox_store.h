 #pragma once

#include <memory>
#include <vector>
#include <thread>
#include <deque>
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/event/events.h"
#include "abstract/IO/io.h"
#include "abstract/adaptor/adaptor.h"
#include "abstract/utils/directory_operation.h"
#include "manager/databox/databox_manager.h"
#include "manager/databox/databox_object.h"
#include "manager/rpc/metadata_rpc/db_protocol.h"
#include "manager/rpc/data_rpc/meta_server.h"
#include "manager/rpc/data_rpc/server.h"
#include "API/dataspace/meta_service.h"



namespace SDS
{

    struct SendEndpoint {
        std::string ip;
        int port;
    };


    struct GetRequest {
        // the client that called get
        Client* client;
        // The ID of the timer that will time out and cause this wait to return 
        int64_t timer;
        // The databox ID involved in this request.
        std::vector<ContentID> databoxIDs;
        // The object information for the objects in this request. This is used in the reply.
        std::unordered_map<ContentID, DataboxObject*, ContentIDHasher> databoxs;

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
            static std::shared_ptr<DataBoxStore> createStore(std::shared_ptr<EventLoop> loop, int64_t systemMemory, Adaptor* adaptor, std::shared_ptr<BasicMetaServer> rpcServer);

            // create a databox object and add it into the dbentry
            bool createDB(const ContentID& cntID, FilePathList &filePath,
                            Client* client);

            
            // delete db objects that have been created
            bool deleteDB(const std::vector<ContentID> &ids);
            

            // get db object
            bool getDB( Client* client, const std::vector<ContentID> &ids,
                     int64_t timeout_ms);

            // return db object with arrow flight
            bool returnDBwithFlight(GetRequest* getReq);
 
            // undersend an databox, this databox is now immutable and can be accessed with flight.
            bool unsendDB(const ContentID &cntID);

            // check if the databox store contains an databox
            bool containDB(const ContentID &cntID);

            // release a client that is no longer using an object
            bool releaseDB(const ContentID &cntID, Client* client);

            // connect a new client to the databox Store
            void connectClient(int listenerSock); 

            // Disconnect a client from the databox Store
            void disconnectClient(Client* client);

            // the main message process 
            Status processMessage(Client* client);


            void addDataServer(std::shared_ptr<BasicDataServer> server);
            void setMetaServer(std::shared_ptr<MetaService> metaService);

            void runServer();

        
    
        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxStore(std::shared_ptr<Impl> impl);
            bool addClientToEntry(DataBoxTableEntry* entry, Client* client);
            bool removeClientFromEntry(DataBoxTableEntry* entry, Client*  client);
            bool prepareTransffer(const ContentID &cntID, DataboxObject* dbObject, SendEndpoint &ep);
            
    };

   


}