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
            std::unordered_map<ContentID, std::vector<GetRequest*>, 
                                ContentIDHasher> databoxGetRequest;
            
            Adaptor* adaptor_;
            std::shared_ptr<BasicMetaServer> rpcServer_;
            std::deque<std::shared_ptr<BasicDataServer>> sendPool_;
            std::shared_ptr<MetaService> metaService_;

            

        public:
            Impl(std::shared_ptr<EventLoop> loop, int64_t systemMemory, 
                                Adaptor *adaptor,  std::shared_ptr<BasicMetaServer> rpcServer) {
                loop_ = std::move(loop);
                storeInfo.memoryCapacity = systemMemory;
                adaptor_ = adaptor;
                rpcServer_ = rpcServer;
            }

            ContentDesc& getMetaIndex(ContentID &cntID) {
                return metaService_->getContentDesc(cntID);
            }


            void setAdaptor(Adaptor *adaptor) {
                adaptor_ = adaptor;
            }

            Adaptor* getAdptor() {
                return adaptor_;
            }

            DataBoxStoreInfo& getStoreInfo() {
                return this->storeInfo;
            }

            std::unordered_map<ContentID, std::vector<GetRequest*>, 
                                ContentIDHasher>& getDBRequest() {
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

            void insertDBObjectEntry(const ContentID &cntID, DataBoxTableEntry *entry)
            {
                storeInfo.databoxs.insert({cntID, entry});
            }

            DataBoxTableEntry* getDBObjectEntry(const ContentID &cntID) {
                auto ret =  storeInfo.databoxs.find(cntID);

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

            void setMetaServer(std::shared_ptr<MetaService> metaService) {
                metaService_ = metaService;
            }

            ~Impl() {}
    };

    DataBoxStore::DataBoxStore(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {
      
    }


    std::shared_ptr<DataBoxStore> DataBoxStore::createStore(std::shared_ptr<EventLoop> loop, int64_t systemMemory, Adaptor *adaptor, std::shared_ptr<BasicMetaServer> rpcServer) {
    
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop, systemMemory, adaptor, rpcServer);
        std::shared_ptr<DataBoxStore> dbStore(new DataBoxStore(impl)); 
        return dbStore;
    }

    DataBoxStore::~DataBoxStore() {}


    
    bool DataBoxStore::createDB(const ContentID &cntID, FilePathList &filePath, Client* client) {

        // There is already an object with the same ID in the databox store
        if(impl_->getStoreInfo().databoxs.count(cntID) != 0) {
            return true;
        }


        // create the databox entry
        DataBoxTableEntry* entry = new DataBoxTableEntry;
        entry->cntID = cntID;
        entry->state = DATABOX_CREATED;
        entry->ptr = nullptr;
    

        // create the actual databox 
        DataboxObject *dbObject = new DataboxObject; 
        dbObject->setDataPath(filePath);
        dbObject->fillData(impl_->getAdptor());
        entry->ptr= dbObject;
        entry->state = DATABOX_FILLED;

        // insert entry into store info
        impl_->insertDBObjectEntry(cntID, entry);

        // record the client who have access this databox object
        addClientToEntry(entry, client);
        return true;
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

    bool DataBoxStore::deleteDB(const std::vector<ContentID> &ids) {

        for(const auto &cntID : ids) {
            auto entry = impl_->getDBObjectEntry(cntID);
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

            // release the realted to databox
            entry->ptr->removeData();
            if(entry->ptr) {
                delete entry->ptr;
            }
            entry->ptr = nullptr;

            // release the related to rpc client
            auto rpcClient = impl_->getRpcServer()->RentOne(cntID.getSpaceID());
            if(rpcClient->DeleteDB(cntID) == true) {

                if(rpcClient->BeLive() == false) {
                    auto ret = rpcClient->WithdrawSend(cntID.getSpaceID());
                    if(ret.ok()) {
                        impl_->returnSender(rpcClient);
                    }
                }
            }

            impl_->getStoreInfo().databoxs.erase(cntID);
            delete entry;
        }
        return true;
    }
            

    // get db object
    bool DataBoxStore::getDB(Client* client, const std::vector<ContentID> &ids,
                int64_t timeout) {

        // create a get request for ths databox
        GetRequest* getReq = new GetRequest(client, ids);
        for(auto cntID : ids) {
            auto entry = impl_->getDBObjectEntry(cntID);

            if(entry && entry->state == DATABOX_FILLED) {
                auto databoxObject = entry->ptr;
                getReq->databoxs[cntID] = databoxObject;
                addClientToEntry(entry, client);
            } else {
                // add a placeholder databox object to the get request to indicate that the object
                // is not present
                DataboxObject* dbObject = new DataboxObject;
                getReq->databoxs[cntID] = dbObject;
                auto dbRequest = impl_->getDBRequest();
                dbRequest[cntID].push_back(getReq);
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

        for(auto cntID : getReq->databoxIDs) {
            SendEndpoint ep;
            auto dbObject = getReq->databoxs[cntID];
       
            if(prepareTransffer(cntID, dbObject, ep) == true) {
                ARROW_LOG(INFO) << "Return databox with arrow flight, prepare your arrow flight client, please";
                unsendDB(cntID);
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
    bool DataBoxStore::unsendDB(const ContentID &cntID) {

        auto entry = impl_->getDBObjectEntry(cntID);

        if(!entry) {
            return false;
        }

        entry->state = DATABOX_UNSEND;
        return true;
    }

    // check if the databox store contains an databox
    bool DataBoxStore::containDB(const ContentID &cntID) {
        auto entry = impl_->getDBObjectEntry(cntID);
        return entry && (entry->state != DATABOX_CREATED) ? OBJECT_FOUND : OBJECT_NOT_FOUND;
    }

    // release a client that is no longer using an object
    bool DataBoxStore::releaseDB(const ContentID &cntID, Client* client) {
        auto entry = impl_->getDBObjectEntry(cntID);

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
        ContentID cntID;
        std::string spaceID;
        std::string timeID;
        std::string varID;

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
                std::string dataPath;
                RETURN_NOT_OK(ReadCreateRequest(input, spaceID, timeID, varID, dataPath));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);



                std::vector<FilePathList> pathListVector;
                getFilePathList(dataPath, pathListVector);

                if(pathListVector.size() > 0) {
                    createDB(cntID, pathListVector[0], client);
                    {
                        auto dbMeta = impl_->getDBObjectEntry(cntID)->ptr->getDBMeta();
                        HANDLE_SIGPIPE(SendCreateReply(client->fd, dbMeta), client->fd);
                    }
                }
            } break;
            case MessageTypeGetRequest: {
                int64_t timeout;
                RETURN_NOT_OK(ReadGetRequest(input, spaceID, timeID, varID, timeout));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                std::vector<ContentID> ids;
                ids.push_back(cntID);
                getDB(client, ids, timeout);
            } break;
            case MessageTypeContaineRequest: {
                RETURN_NOT_OK(ReadContainRequest(input, spaceID, timeID, varID));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                bool ret = containDB(cntID);
                {
                    HANDLE_SIGPIPE(SendContainReply(client->fd, ret), client->fd);
                }
            } break;
            case MessageTypeReleaseRequest: {
                RETURN_NOT_OK(ReadReleaseRequest(input, spaceID, timeID, varID));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                bool ret = releaseDB(cntID, client);
                {
                    HANDLE_SIGPIPE(SendReleaseReply(client->fd, ret), client->fd);
                }
            } break;
            case MessageTypeDeleteRequest: {
                RETURN_NOT_OK(ReadDeleteRequest(input, spaceID, timeID, varID));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                std::vector<ContentID> ids;
                ids.push_back(cntID);
                bool ret = deleteDB(ids);
                {
                    HANDLE_SIGPIPE(SendDeleteReply(client->fd, ret), client->fd);
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

    void DataBoxStore::setMetaServer(std::shared_ptr<MetaService> metaService) {
        impl_->setMetaServer(metaService);
    }

    void DataBoxStore::runServer() {
        impl_->getLoop()->run();
    }


}