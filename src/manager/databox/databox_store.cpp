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
            
            std::shared_ptr<Adaptor> adaptor_;

        public:
            Impl(std::shared_ptr<EventLoop> loop, int64_t systemMemory) {
                loop_ = std::move(loop);
                storeInfo.memoryCapacity = systemMemory;
            }

            void setAdaptor(std::shared_ptr<Adaptor> adaptor) {
                adaptor_ = adaptor;
            }

            std::shared_ptr<Adaptor> getAdptor() {
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
            
            ~Impl() {}
    };

    DataBoxStore::DataBoxStore(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {}


    std::shared_ptr<DataBoxStore> DataBoxStore::createStore(std::shared_ptr<EventLoop> loop,
                                                             int64_t systemMemory) {
    
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop, systemMemory);
        std::shared_ptr<DataBoxStore> dbStore(new DataBoxStore(impl)); 
        return dbStore;
    }
    
    bool DataBoxStore::createDB(const ContentID &cntID, FilePathList &filePath,
                        std::shared_ptr<Client> client, std::shared_ptr<DataboxObject> result) {

        // There is already an object with the same ID in the databox store
        if(impl_->getStoreInfo().databoxs.count(cntID) != 0) {
            return false;
        }

        // create the databox entry
        std::unique_ptr<DataBoxTableEntry> entry(new DataBoxTableEntry());
        entry->cntID = cntID;
        entry->state = DATABOX_CREATED;

        // insert entry into store info
        impl_->getStoreInfo().databoxs[cntID] = std::move(entry);

        // create the actual databox 
        result->setDataPath(filePath);
        result->fillData(impl_->getAdptor());
        entry->ptr = result;

        // record the client who have access this databox object
        addClientToEntry(impl_->getStoreInfo().databoxs[cntID].get(), client);
    }


    bool DataBoxStore::addClientToEntry(DataBoxTableEntry *entry, 
                                        std::shared_ptr<Client> client) {
    
        if(entry->clients.find(client) != entry->clients.end()) {
            return false;
        }
        entry->clients.insert(client);
    }

    bool DataBoxStore::removeClientFromEntry(DataBoxTableEntry* entry, std::shared_ptr<Client> client) {
        auto it = entry->clients.find(client);
        
        if (it != entry->clients.end()) {
            entry->clients.erase(it);
            return true;
        }
        return false;
    }

    DataBoxTableEntry* DataBoxStore::getDBObjectEntry(const ContentID &cntID) {
        auto it = impl_->getStoreInfo().databoxs.find(cntID);

        if(it == impl_->getStoreInfo().databoxs.end()) {
            return nullptr;
        }
        return it->second.get();
    }



             
    bool DataBoxStore::deleteDB(const std::vector<ContentID> &ids) {

        for(const auto &cntID : ids) {
            auto entry = getDBObjectEntry(cntID);
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
    bool DataBoxStore::getDB(std::shared_ptr<Client> client, const std::vector<ContentID> &ids,
                int64_t timeout_ms) {

        // create a get request for ths databox
        GetRequest* getReq = new GetRequest(client.get(), ids);
        for(auto cntID : ids) {
            auto entry = getDBObjectEntry(cntID);

            if(entry && entry->state == DATABOX_SEALED) {
                auto databoxObject = entry->ptr.get();
                getReq->databoxs[cntID] = *databoxObject;
                addClientToEntry(entry, client);
            } else {
                // add a placeholder databox object to the get request to indicate that the object
                // is not present
                DataboxObject dbObject;
                getReq->databoxs[cntID] = dbObject;
                auto dbRequest = impl_->getDBRequest();
                dbRequest[cntID].push_back(getReq);
            }
        }
        return true;
    }
 
    // seal an databox, this databox is now immutable and can be accessed with get.
    bool DataBoxStore::sealDB(const ContentID &cntID) {

        auto entry = getDBObjectEntry(cntID);

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
        auto entry = getDBObjectEntry(cntID);
        return entry && (entry->state == DATABOX_SEALED) ? OBJECT_FOUND : OBJECT_NOT_FOUND;
    }

    // release a client that is no longer using an object
    bool DataBoxStore::releaseDB(const ContentID &cntID, std::shared_ptr<Client> client) {
        auto entry = getDBObjectEntry(cntID);

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

        std::shared_ptr<Client> client_ptr(client);

        for(const auto& entry : impl_->getStoreInfo().databoxs) {
            removeClientFromEntry(entry.second.get(), client_ptr);
        }

        delete client;
        client_ptr.reset();
    }
 
    arrow::Status DataBoxStore::processMessage(Client* client) {
        int64_t type;
        Status s = ReadMessage(client->fd, &type, &(impl_->getInputBuffer()));
        assert(s.ok() || s.IsIOError());

        uint8_t* input = impl_->getInputBuffer().data();
        ContentID cntID;
        DataboxObject  dbObject;

        // Process the different types of requests.
        switch (type) {

        }
        return Status::OK();
        
    }







}