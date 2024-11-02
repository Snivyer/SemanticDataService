#include "manager/databox/databox_store.h"

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
            std::shared_ptr<BasicDataServer> dataServer_;

            

        public:
            Impl(std::shared_ptr<EventLoop> loop, int64_t systemMemory, Adaptor *adaptor) {
                loop_ = std::move(loop);
                storeInfo.memoryCapacity = systemMemory;
                adaptor_ = adaptor;
            }

            arrow::Status runDataFlight(std::string ip, int port, DataboxObject* dbObject) {

                dataServer_ = BasicDataServer::Create(dbObject);
                
                arrow::flight::Location bind_location;
                ARROW_RETURN_NOT_OK(
                    arrow::flight::Location::ForGrpcTcp(ip, port).Value(&bind_location)
                );
                arrow::flight::FlightServerOptions options(bind_location);
                
                ARROW_RETURN_NOT_OK(dataServer_->Init(options));
                ARROW_RETURN_NOT_OK(dataServer_ ->SetShutdownOnSignals({SIGTERM}));
                ARROW_LOG(INFO) << "Server is running at " << bind_location.ToString();
                ARROW_RETURN_NOT_OK(dataServer_->Serve());
                return arrow::Status::OK();
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
            
            ~Impl() {}
    };

    DataBoxStore::DataBoxStore(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {}


    std::shared_ptr<DataBoxStore> DataBoxStore::createStore(std::shared_ptr<EventLoop> loop,
                                                             int64_t systemMemory, 
                                                             Adaptor *adaptor) {
    
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop, systemMemory, adaptor);
        std::shared_ptr<DataBoxStore> dbStore(new DataBoxStore(impl)); 
        return dbStore;
    }

    DataBoxStore::~DataBoxStore() {}


    
    bool DataBoxStore::createDB(const ContentID &cntID, FilePathList &filePath, Client* client, DataboxObject* &result) {

        // There is already an object with the same ID in the databox store
        if(impl_->getStoreInfo().databoxs.count(cntID) != 0) {
            return true;
        }


        // create the databox entry
        DataBoxTableEntry* entry = new DataBoxTableEntry;
        entry->cntID = cntID;
        entry->state = DATABOX_SEALED;

        // create the actual databox 
        result->setDataPath(filePath);
        result->fillData(impl_->getAdptor());
        entry->ptr= result;

        // insert entry into store info
        impl_->insertDBObjectEntry(cntID, entry);

        // record the client who have access this databox object
        addClientToEntry(entry, client);
        return true;
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

            // to delete an databox it must have been sealed
            if(entry->state != DATABOX_SEALED) {
                continue;
            }

            // to delete an databox, there must be no clients currently using it
            if(entry->clients.size() > 0) {
                continue;
            }

            entry->ptr->removeData();
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

            if(entry && entry->state == DATABOX_SEALED) {
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
            returnDBwithFlight(getReq);
        } else if(timeout == -1) {
            getReq->timer = impl_->getLoop()->add_timer(timeout, [this, getReq](int64_t timerID) {
                returnDBwithFlight(getReq);
                return kEventLoopTimerDone;
            });

        }
        return true;
    }

    bool DataBoxStore::returnDBwithFlight(GetRequest* getReq) {

        std::string ip = "0.0.0.0";
        int port = 4444;
        SendGetReply(getReq->client->fd, ip, port);
        ARROW_LOG(INFO) << "Return databox with arrow flight, prepare your arrow flight client, please";
        
        for(auto cntID : getReq->databoxIDs) {
            auto dbObject = getReq->databoxs[cntID];
            impl_->runDataFlight(ip, port, dbObject);
        }

        return true;
    }
 
    // seal an databox, this databox is now immutable and can be accessed with get.
    bool DataBoxStore::sealDB(const ContentID &cntID) {

        auto entry = impl_->getDBObjectEntry(cntID);

        if(!entry) {
            return false;
        }

        if(entry->state == DATABOX_SEALED) {
            return false;
        }

        entry->state = DATABOX_SEALED;
    }

    // check if the databox store contains an databox
    bool DataBoxStore::containDB(const ContentID &cntID) {
        auto entry = impl_->getDBObjectEntry(cntID);
        return entry && (entry->state == DATABOX_SEALED) ? OBJECT_FOUND : OBJECT_NOT_FOUND;
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
        DataboxObject *dbObject = new DataboxObject; 

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
                std::string spaceID;
                std::string timeID;
                std::string varID;
                std::string dataPath;
                RETURN_NOT_OK(ReadCreateRequest(input, spaceID, timeID, varID, dataPath));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                std::vector<FilePathList> pathListVector;
                getFilePathList(dataPath, pathListVector);

                if(pathListVector.size() > 0) {
                    dbObject->setDataPath(pathListVector[0]);
                    createDB(cntID, pathListVector[0], client, dbObject);
                    {
                        HANDLE_SIGPIPE(SendCreateReply(client->fd, dbObject->getDBMeta()), client->fd);
                    }
                }
            } break;
            case MessageTypeGetRequest: {
                std::string spaceID;
                std::string timeID;
                std::string varID;
                int64_t timeout;
            
                RETURN_NOT_OK(ReadGetRequest(input, spaceID, timeID, varID, timeout));
                cntID.setSpaceID(spaceID);
                cntID.setTimeID(timeID);
                cntID.setVarID(varID);

                std::vector<ContentID> ids;
                ids.push_back(cntID);
                bool ret = getDB(client, ids, timeout);

            } break;
      
            default:
                ARROW_CHECK(0);
        }
        return Status::OK();
        
    }







}