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
#include "abstract/adaptor/adaptor_factory.h"
#include "abstract/utils/directory_operation.h"
#include "manager/databox/databox_manager.h"
#include "manager/databox/databox_object.h"
#include "manager/rpc/metadata_rpc/db_protocol.h"
#include "manager/rpc/data_rpc/meta_server.h"
#include "manager/rpc/data_rpc/server.h"
#include "API/dataspace/meta_service_client.h"



namespace SDS
{
    static size_t globalDataBoxID = 0;

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
        std::vector<size_t> databoxIDs;
        // The object information for the objects in this request. This is used in the reply.
        std::unordered_map<size_t, DataboxObject*> databoxs;
        // todo: 这里是不是没必要缓存呀

        GetRequest(Client *client, const std::vector<size_t> &ids) {
            this->client = client;
            this->timer = -1;
            for(auto id : ids) {
                this->databoxIDs.push_back(id);
            }
        }
    };


    class DataBoxStore {
        public:
            ~DataBoxStore();
            static std::shared_ptr<DataBoxStore> createStore(std::shared_ptr<EventLoop> loop, int64_t systemMemory, 
                                                                std::shared_ptr<BasicMetaServer> rpcServer);

            // create a databox object and add it into the dbentry
            size_t createDB(const ContentID &cntID, ContentDesc &cntDesc,
                             StoreDesc &stoDesc, FilePathList &filePath, Client* client);

            
            // delete db objects that have been created
            bool deleteDB(std::vector<size_t> &ids);
            

            // get db object
            bool getDB( Client* client, std::vector<size_t> &ids, int64_t timeout_ms);

            // return db object with arrow flight
            bool returnDBwithFlight(GetRequest* getReq);
 
            // undersend an databox, this databox is now immutable and can be accessed with flight.
            bool unsendDB(size_t id);

            // check if the databox store contains an databox
            bool containDB(size_t id);

            // release a client that is no longer using an object
            bool releaseDB(size_t id, Client* client);

            // connect a new client to the databox Store
            void connectClient(int listenerSock); 

            // Disconnect a client from the databox Store
            void disconnectClient(Client* client);

            // the main message process 
            Status processMessage(Client* client);

            void connectToMetaService();
            void disconnectToMetaService();


            void addDataServer(std::shared_ptr<BasicDataServer> server);

            void runServer();

           

        
    
        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxStore(std::shared_ptr<Impl> impl);
            bool addClientToEntry(DataBoxTableEntry* entry, Client* client);
            bool removeClientFromEntry(DataBoxTableEntry* entry, Client*  client);
            bool prepareTransffer(const ContentID &cntID, DataboxObject* dbObject, SendEndpoint &ep);
            bool getContentIDByDBID(size_t dbID, ContentID &cntID);
            
    };

   


}