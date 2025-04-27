#include "API/dataspace/databox_store.h"

namespace SDS {

    class DataBoxStore::Impl {
        private:
            // Event loop of the databox store
            std::shared_ptr<EventLoop> loop_;
           
            // databox store information, including the databox table
            DataBoxStoreInfo storeInfo;

            std::vector<uint8_t> inputBuffer;
            // A hash table mapping object IDs to a vector of the get request that 
            // are waitting for the object to arrive
            std::unordered_map<size_t, std::vector<GetRequest*>> databoxGetRequest;
            
            std::shared_ptr<BasicMetaServer> rpcServer_;
            std::deque<std::shared_ptr<BasicDataServer>> sendPool_;
            

            

        public:
            std::shared_ptr<MetaServiceClient> metaclient_;
            Impl(std::shared_ptr<EventLoop> loop, int64_t systemMemory, 
                                std::shared_ptr<BasicMetaServer> rpcServer) {
                loop_ = std::move(loop);
                storeInfo.memoryCapacity = systemMemory;
                rpcServer_ = rpcServer;
                metaclient_ = MetaServiceClient::createClient();
            }

            DataBoxStoreInfo& getStoreInfo() {
                return this->storeInfo;
            }

            std::unordered_map<size_t, std::vector<GetRequest*>>& getDBRequest() {
                return this->databoxGetRequest;                        
            }

            std::shared_ptr<EventLoop> getLoop() {
                return this->loop_;
            }

            void setRpcServer(std::shared_ptr<BasicMetaServer> rpcServer) {
                rpcServer_ = rpcServer;
            }

            std::shared_ptr<BasicMetaServer> getRpcServer() {
                return rpcServer_;
            }

            std::vector<uint8_t> & getInputBuffer() {
                return this->inputBuffer;
            }

            void insertDBObjectEntry(DataBoxTableEntry *entry) {
                storeInfo.databoxs.insert({entry->dbID, entry});
            }

            DataBoxTableEntry* getDBObjectEntry(size_t dbID) {
                auto ret =  storeInfo.databoxs.find(dbID);
                if(ret != storeInfo.databoxs.end()) {
                    return ret->second;
                }
                return nullptr;
            }

            std::shared_ptr<BasicDataServer> rentSender() {

                if( sendPool_.size() > 0) {
                    std::shared_ptr<BasicDataServer> client = sendPool_.front();
                    sendPool_.pop_front();
                    return client;
                } else {
                    std::shared_ptr<BasicDataServer> client(nullptr);
                    return client;
                }
            }

            void returnSender(std::shared_ptr<BasicDataServer> server) {
                sendPool_.push_back(server);
            }

            ~Impl() {}
    };

    DataBoxStore::DataBoxStore(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {
      
    }


    std::shared_ptr<DataBoxStore> DataBoxStore::createStore(std::shared_ptr<EventLoop> loop, int64_t systemMemory, std::shared_ptr<BasicMetaServer> rpcServer) {
    
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop, systemMemory, rpcServer);
        std::shared_ptr<DataBoxStore> dbStore(new DataBoxStore(impl)); 
        return dbStore;
    }

    DataBoxStore::~DataBoxStore() {}

    size_t DataBoxStore::createDB(const ContentID &cntID, ContentDesc &cntDesc,
                                        StoreDesc &stoDesc, FilePathList &filePath, Client* client) {

        // create the databox entry
        globalDataBoxID += 1;
        DataBoxTableEntry* entry = new DataBoxTableEntry;
        entry->cntID = cntID;
        entry->dbID = globalDataBoxID;
        entry->state = DATABOX_CREATED;
        entry->ptr = nullptr;

        // create the actual databox 
        DataboxObject *dbObject = new DataboxObject; 
        Adaptor* adaptor = AdaptorFactory::getAdaptor(stoDesc.kind, stoDesc.conConf, &filePath);
        dbObject->fillData(adaptor, cntDesc, entry->dbID);
        entry->ptr= dbObject;
        entry->state = DATABOX_FILLED;

        // insert entry into store info
        impl_->insertDBObjectEntry(entry);

        // record the client who have access this databox object
        addClientToEntry(entry, client);
        return entry->dbID;
    }

    bool DataBoxStore::prepareTransffer(const ContentID &cntID, DataboxObject* dbObject, SendEndpoint &ep) {

        if(dbObject == nullptr) {
            return false;
        }

        // if there are a rpc server that is rensposible to send the databox with the same spaceID, going on
        auto rpcClient = impl_->getRpcServer()->RentOne(cntID.getSpaceID());
        if(rpcClient.get() == nullptr) {

            // there is no sender can be used to send this type data box
            rpcClient = impl_->rentSender();
            if(rpcClient.get() == nullptr) {
                ep.ip = "X.X.X.X";
                ep.port = 0;
                ARROW_LOG(INFO) << "There are no more idle rpc client to transffer data, wait please!";
                return false;
            }

            // notify the rpc metadata server to rent this rpc client
            impl_->getRpcServer()->CreateRent(cntID.getSpaceID(), rpcClient);
        }

        rpcClient->PrepareSend(cntID, dbObject);
        ep.port = rpcClient->getRunPort();
        ep.ip = rpcClient->getRunIp();
        return  true;
        
    }


    bool DataBoxStore::addClientToEntry(DataBoxTableEntry *entry, 
                                        Client* client) {
        if(entry->clients.find(client) != entry->clients.end()) {
            return false;
        }
        entry->clients.insert(client);
    }

    bool DataBoxStore::removeClientFromEntry(DataBoxTableEntry* entry, Client* client) {
        auto it = entry->clients.find(client);
        
        if (it != entry->clients.end()) {
            entry->clients.erase(it);
            return true;
        }
        return false;
    }

    bool DataBoxStore::deleteDB(std::vector<size_t> &ids) {
        for(size_t id : ids) {
            auto entry = impl_->getDBObjectEntry(id);
            if(!entry) {
                continue;
            }

            // delete an databox it must have been filled
            if(entry->state == DATABOX_CREATED) {
                continue;
            }

            // delete an databox, there must be no clients currently using it
            if(entry->clients.size() > 0) {
                continue;
            }

            // release the related to rpc client
            auto rpcClient = impl_->getRpcServer()->RentOne(entry->cntID.getSpaceID());
            if(rpcClient->DeleteDB(entry->cntID.getSpaceID())) {
                if(!rpcClient->BeLive()) {
                    auto ret = rpcClient->WithdrawSend(entry->cntID.getSpaceID());
                    if(ret.ok()) {
                        impl_->returnSender(rpcClient);
                    }
                }
            }

            // release the realted to databox
            entry->ptr->removeData();
            if(entry->ptr) {
                delete entry->ptr;
            }
            entry->ptr = nullptr;

            impl_->getStoreInfo().databoxs.erase(id);
            delete entry;
        }
        return true;
    }
            

    // get db object
    bool DataBoxStore::getDB(Client* client, std::vector<size_t> &ids, int64_t timeout) {

        // create a get request for ths databox
        GetRequest* getReq = new GetRequest(client, ids);
        for(auto id : ids) {
            auto entry = impl_->getDBObjectEntry(id);
            if(entry && entry->state == DATABOX_FILLED) {
                auto databoxObject = entry->ptr;
                getReq->databoxs[id] = databoxObject;
                addClientToEntry(entry, client);
            } else {
                // add a placeholder databox object to the get request to indicate that the object
                // is not present
                DataboxObject* dbObject = new DataboxObject;
                getReq->databoxs[id] = dbObject;
                auto dbRequest = impl_->getDBRequest();
                dbRequest[id].push_back(getReq);
            }
        }

        if(timeout == 0) {
            return returnDBwithFlight(getReq);
        } else if(timeout != -1) {
            getReq->timer = impl_->getLoop()->add_timer(timeout, [this, getReq](int64_t timerID) {
                returnDBwithFlight(getReq);
                return kEventLoopTimerDone;
            });
        }

        return true;
    }

    bool DataBoxStore::returnDBwithFlight(GetRequest* getReq) {

        for(size_t id : getReq->databoxIDs) {
            SendEndpoint ep;
            auto entry = impl_->getDBObjectEntry(id);
            if(prepareTransffer(entry->cntID, entry->ptr, ep)) {
                ARROW_LOG(INFO) << "Return databox with arrow flight, prepare your arrow flight client, please";
                unsendDB(id);
            }
            SendGetReply(getReq->client->fd, ep.ip, ep.port), getReq->client->fd;
           
        }

        if(getReq->timer != -1) {
            ARROW_CHECK(impl_->getLoop()->remove_timer(getReq->timer) == AE_OK);
        }
        delete getReq;
        return true;
    }
 
    // seal an databox, this databox is now immutable and can be accessed with get.
    bool DataBoxStore::unsendDB(size_t id) {
        auto entry = impl_->getDBObjectEntry(id);
        if(!entry) {
            return false;
        }

        entry->state = DATABOX_UNSEND;
        return true;
    }

    // check if the databox store contains an databox
    bool DataBoxStore::containDB(size_t id) {
        auto entry = impl_->getDBObjectEntry(id);
        return entry && (entry->state != DATABOX_CREATED) ? OBJECT_FOUND : OBJECT_NOT_FOUND;
    }

    // release a client that is no longer using an object
    bool DataBoxStore::releaseDB(size_t id, Client* client) {
        auto entry = impl_->getDBObjectEntry(id);
        if(!entry) {
            return false;
        }

        return removeClientFromEntry(entry, client);
    }
      
    void DataBoxStore::connectClient(int listenerSock) {
        int clientFd = AcceptClient(listenerSock);
        Client* client = new Client(clientFd);

        impl_->getLoop()->add_file_event(clientFd, kEventLoopRead, [this, client](int events) {
            processMessage(client);
        });
    }
   
    void DataBoxStore::disconnectClient(Client* client) {
        assert(client != nullptr);
        assert(client->fd > 0);
        
        impl_->getLoop()->remove_file_event(client->fd);
        close(client->fd);

        for(const auto& entry : impl_->getStoreInfo().databoxs) {
            removeClientFromEntry(entry.second, client);
        }
    }
 
    arrow::Status DataBoxStore::processMessage(Client* client) {
        int64_t type;
        Status s = ReadMessage(client->fd, &type, &(impl_->getInputBuffer()));
        assert(s.ok() || s.IsIOError());

        uint8_t* input = impl_->getInputBuffer().data();
    
        // Process the different types of requests.
        switch (type) {
            case MessageTypeConnectRequest: {
                HANDLE_SIGPIPE(
                SendConnectReply(client->fd, impl_->getStoreInfo().memoryCapacity), client->fd);
            } break;
            case DISCONNECT_CLIENT:
                ARROW_LOG(DEBUG) << "Disconnecting client on fd " << client->fd;
                disconnectClient(client);
                break;
            case MessageTypeCreateRequest: {
                ContentID cntID;
                ContentDesc cntDesc;
                StoreDesc stoDesc;
                FilePathList pathList;
                RETURN_NOT_OK(ReadCreateRequest(input, cntID, cntDesc, stoDesc, pathList));
                size_t id = createDB(cntID, cntDesc, stoDesc, pathList, client);
                {
                    auto dbMeta = impl_->getDBObjectEntry(id)->ptr->getDBMeta();
                    HANDLE_SIGPIPE(SendCreateReply(client->fd, dbMeta), client->fd);
                }
            } break;
            case MessageTypeGetRequest: {
                int64_t timeout;
                std::vector<size_t> ids;
                RETURN_NOT_OK(ReadGetRequest(input, ids, timeout));
                getDB(client, ids, timeout);
            } break;
            case MessageTypeContaineRequest: {
                size_t id;
                RETURN_NOT_OK(ReadContainRequest(input, id));
                bool ret = containDB(id);
                {
                    HANDLE_SIGPIPE(SendContainReply(client->fd, ret), client->fd);
                }
            } break;
            case MessageTypeReleaseRequest: {
                size_t id;
                RETURN_NOT_OK(ReadReleaseRequest(input, id));
                bool ret = releaseDB(id, client);
                {
                    HANDLE_SIGPIPE(SendReleaseReply(client->fd, ret), client->fd);
                }
            } break;
            case MessageTypeDeleteRequest: {
                std::vector<size_t> ids;
                RETURN_NOT_OK(ReadDeleteRequest(input, ids));
                bool ret = deleteDB(ids);
                {
                    HANDLE_SIGPIPE(SendDeleteReply(client->fd, ret), client->fd);
                }
            } break;
            case MessageTypeGetContentIDRequest: {
                size_t dbID;
                ContentID cntID;
                RETURN_NOT_OK(ReadGetContentIDRequest(input, dbID));
                bool ret = getContentIDByDBID(dbID, cntID);
                {
                    HANDLE_SIGPIPE(SendGetContentIDReply(client->fd, cntID), client->fd);
                }

            } break;
            default:
                ARROW_CHECK(0);
        }
        return Status::OK();
        
    }

    void  DataBoxStore::addDataServer(std::shared_ptr<BasicDataServer> server) {
        impl_->returnSender(server);
    }

    void DataBoxStore::runServer() {
        impl_->getLoop()->run();
    }

    void DataBoxStore::connectToMetaService() {
        impl_->metaclient_ = MetaServiceClient::createClient();
        Status ret = impl_->metaclient_->connect("/tmp/meta", "");
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "meta client connect success!  \n";
        } 
    }

    void DataBoxStore::DataBoxStore::disconnectToMetaService() {
        Status ret = impl_->metaclient_->disconnect();
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "meta client disconnect success!  \n";
        } 
    }

    bool DataBoxStore::getContentIDByDBID(size_t dbID, ContentID &cntID) {
        cntID = impl_->getDBObjectEntry(dbID)->cntID;
        return true;
    }
}