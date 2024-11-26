#include "API/dataspace/meta_service.h"


namespace SDS {

    class MetaService::Impl {
        private:
            // Event loop of the metadata service
            std::shared_ptr<EventLoop> loop_;

            // Metadata service info
            MetaServiceInfo metaInfo_;

            // Metadata request buffer
            std::vector<uint8_t> inputBuffer;

            // semantic space manager
            std::shared_ptr<SemanticSpaceManager> SemanticManager_;

            // storage space manager
            std::shared_ptr<StorageSpaceManager> StorageManager_;

            // metadata index
            std::unordered_map<ContentID, ContentDesc, ContentIDHasher> metaIndex_;

           
        public:
            Impl(std::shared_ptr<EventLoop> loop) {
                loop_ = std::move(loop);
            }

            std::shared_ptr<EventLoop> getLoop() {
                return this->loop_;
            }

            MetaServiceInfo& getMetaServiceInfo() {
                return this->metaInfo_;
            }

            std::vector<uint8_t> & getInputBuffer() {
                return this->inputBuffer;
            }

            std::shared_ptr<SemanticSpaceManager> getSemanticManager() {
                return this->SemanticManager_;
            }

            std::shared_ptr<StorageSpaceManager> getStorageManager() {
                return this->StorageManager_;
            }

            void setMetaIndex(std::unordered_map<ContentID, ContentDesc, ContentIDHasher> metaIndex) {
                this->metaIndex_ = metaIndex;
            }

            void addMetaIndex(ContentID &cntID, ContentDesc &cntDesc) {
                metaIndex_[cntID] = cntDesc;
            }

            void insertSemenaticSpaceEntry(std::string spaceName, SemanticSpaceEntry* entry) {
                this->metaInfo_.SemanticSpaceInfo.insert({spaceName, entry});
            }

            void insertStorageSpaceEntry(std::string spaceName, StorageSpaceEntry* entry) {
                this->metaInfo_.StorageSpaceInfo.insert({spaceName, entry});
            }
            
    };

    MetaService::~MetaService() {

    }
    
    
    std::shared_ptr<MetaService> MetaService::createMetaService(std::shared_ptr<EventLoop> loop, std::unordered_map<ContentID, ContentDesc, ContentIDHasher> metaIndex) {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop);
        impl->setMetaIndex(metaIndex);
        std::shared_ptr<MetaService> service(new MetaService(impl)); 
        return service;
    }

    MetaService::MetaService(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {

    }

    size_t MetaService::createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames,  MetaClient* client) {
        
        auto SemanticSpaceInfo = impl_->getMetaServiceInfo().SemanticSpaceInfo;

        if(SemanticSpaceInfo.count(SSName) != 0) {
           return SemanticSpaceInfo[SSName]->space->spaceID;
        }

        auto manager = impl_->getSemanticManager();
        size_t spaceID = manager->createSemanticSpace(SSName, geoNames);

        if(spaceID != 0) {
            SemanticSpace* space = manager->getSpaceByID(std::to_string(spaceID));
            SemanticSpaceEntry* entry = new SemanticSpaceEntry;
            entry->semanticSpaceName = SSName;
            entry->space = space;
            impl_->insertSemenaticSpaceEntry(SSName, entry);
            addClientToSemanticSpaceEntry(entry, client);
            return spaceID;
        }
        return 0;
    }

    size_t MetaService::createStorageSpace(std::string spaceID, StoreTemplate &storeInfo, std::string storekind, MetaClient* client) {

        std::string SSName = storeInfo.SSName;
        auto StorageSpaceInfo = impl_->getMetaServiceInfo().StorageSpaceInfo;
        if(StorageSpaceInfo.count(SSName) != 0) {
           return StorageSpaceInfo[SSName]->space->storageID;
        }

        if(storekind == "Ceph") {
            storeInfo.kind = SDS::StoreSpaceKind::Ceph;
        } else if (storekind == "Lustre") {
            storeInfo.kind = StoreSpaceKind::Lustre;
        } else if (storekind == "BB") {
            storeInfo.kind = StoreSpaceKind::BB;
        } else {
            storeInfo.kind = StoreSpaceKind::None;
        }

        auto manager = impl_->getStorageManager();
        size_t storageID = manager->createStorageSpace(storeInfo);

        if(storageID != 0) {
            StorageSpace* space = manager->getSpaceByID(storageID);
            StorageSpaceEntry* entry = new StorageSpaceEntry;
            entry->storageSpaceName = SSName;
            entry->space = space;
            impl_->insertStorageSpaceEntry(SSName, entry);
            addClientToStorageSpaceEntry(entry, client);
            return storageID;
        }
    }

    bool MetaService::createContentIndex(std::string SemanticSpaceName, std::string StoreSpaceName, std::string dirName) {

        // get spaceID
        auto SemanticSpaceInfo = impl_->getMetaServiceInfo().SemanticSpaceInfo;
        if(SemanticSpaceInfo.count(SemanticSpaceName) == 0) {
            ARROW_LOG(DEBUG) << "Cannot Find Target Semantic Space Name.";
            return false;
        }
        size_t spaceID =  SemanticSpaceInfo[SemanticSpaceName]->space->spaceID;

        // get storageID
        auto StorageSpaceInfo = impl_->getMetaServiceInfo().StorageSpaceInfo;
        if(StorageSpaceInfo.count(StoreSpaceName) != 0) {
            ARROW_LOG(DEBUG) << "Cannot Find Target Storage Space Name.";
            return false;
        }
        size_t storageID = StorageSpaceInfo[StoreSpaceName]->space->storageID;
        return createContentIndexInternal(std::to_string(spaceID), storageID, dirName);

    }

    bool MetaService::searchContentIndex(std::vector<std::string> geoNames, std::vector<std::string> times, std::vector<std::string> varNames, 
                            ContentID& cntID, std::string varGroupName) {
        
        std::string spaceID;
        std::string timeID;
        std::string varID;   

        auto semanticManager = impl_->getSemanticManager();
        auto storageManager = impl_->getStorageManager();

        // extract content description
        ContentDesc cntDesc;
        auto metaManager = semanticManager->getContentMeta();
       

        // firstly, get spaceID by search space index
        if(metaManager->extractSSDesc(cntDesc, geoNames) == false) {
            ARROW_LOG(DEBUG) << "cannot find the actual geoName";
            return false;
        }

        auto spaceIndex = semanticManager->getSpaceIndex();
        SpaceNode* spaceNode = nullptr;
        spaceIndex->search(cntDesc.ssDesc.adCode, spaceNode);

        if(!spaceNode) {
            ARROW_LOG(DEBUG) << "There is no object space in space index.";
        } 
        spaceID = spaceNode->getCompleteSpaceID();
        auto semanticSpace = semanticManager->getSpaceByID(spaceID);

        // secondly, get timeID by search space index
        auto timeIndex = semanticManager->getTimeIndex();
        TimeSlotNode* timeSlotNode = nullptr;

        if( metaManager->extractTSDesc(cntDesc, times)) {
            // user have set the required time
            time_t reportT = mktime(&(cntDesc.tsDesc.reportT));
            timeIndex->search(reportT, timeSlotNode);  

            if(timeSlotNode) {
                timeID = timeSlotNode->getTimeSlotID();
            }
        } else {
            // inherit the time ID under the object semanticSpace
            timeID = semanticSpace->cntID.getTimeID();
            cntDesc.copyTimeSlotDesc(semanticSpace->cntDesc);
        }

        // thirdly, get varID by search var index 
        auto varIndex = semanticManager->getVarIndex();
        VarListNode* varListNode = nullptr;
        varIndex->search(varGroupName, varListNode);

        if(varListNode) {
            auto varList = varListNode->varIndex;
            if(metaManager->extractVLDesc(cntDesc, varNames, varList)) {
                // user have set required var
                varID = varListNode->getVarListID();
            } else {
                // inherit the var ID under the object semanticSpace
                varID = semanticSpace->cntID.getVarID();
                cntDesc.copyVarListDesc(semanticSpace->cntDesc);
        
            }
        }

        // finally, clean up
        cntID.setSpaceID(spaceID);
        cntID.setTimeID(timeID);
        cntID.setVarID(varID);
        cntID.storeIDs.push_back(semanticSpace->cntID.getBestStoID());
        impl_->addMetaIndex(cntID, cntDesc);


    }
    bool MetaService::addClientToSemanticSpaceEntry(SemanticSpaceEntry *entry, MetaClient* client) {
    
        if(entry->clients.find(client) != entry->clients.end()) {
            return false;
        }
        entry->clients.insert(client);
    }

    bool MetaService::addClientToStorageSpaceEntry(StorageSpaceEntry *entry, MetaClient* client) {
    
        if(entry->clients.find(client) != entry->clients.end()) {
            return false;
        }
        entry->clients.insert(client);
    }

    bool MetaService::removeClientFromSemanticSpaceEntry(SemanticSpaceEntry* entry, MetaClient* client) {
        auto it = entry->clients.find(client);
        
        if (it != entry->clients.end()) {
            entry->clients.erase(it);
            return true;
        }
        return false;
    }

    bool MetaService::removeClientFromStorageSpaceEntry(StorageSpaceEntry* entry, MetaClient* client) {
        auto it = entry->clients.find(client);
        
        if (it != entry->clients.end()) {
            entry->clients.erase(it);
            return true;
        }
        return false;
    }

    void MetaService::connectClient(int listenerSock) {
        int clientFd = AcceptClient(listenerSock);
        MetaClient* client = new MetaClient(clientFd);

        impl_->getLoop()->add_file_event(clientFd, kEventLoopRead, [this, client](int events) {
            processMessage(client);
        });
    }
   
    void MetaService::disconnectClient(MetaClient* client) {
        assert(client != nullptr);
        assert(client->fd > 0);
        
        impl_->getLoop()->remove_file_event(client->fd);
        close(client->fd);

        for(const auto& entry : impl_->getMetaServiceInfo().SemanticSpaceInfo) {
            removeClientFromSemanticSpaceEntry(entry.second, client);
        }

        for(const auto& entry : impl_->getMetaServiceInfo().StorageSpaceInfo) {
            removeClientFromStorageSpaceEntry(entry.second, client);
        }
    }

    Status MetaService::processMessage(MetaClient* client) {
        int64_t type;
        Status s = ReadMessage(client->fd, &type, &(impl_->getInputBuffer()));
        assert(s.ok() || s.IsIOError());

        uint8_t* input = impl_->getInputBuffer().data();

        // Process the different types of requests.
        switch (type) {
            case MessageTypeMetaConnectRequest: {
                HANDLE_SIGPIPE(
                SendMetaConnectReply(client->fd, 1), client->fd);
            } break;
            case DISCONNECT_CLIENT:
                ARROW_LOG(DEBUG) << "Disconnecting client on fd " << client->fd;
                disconnectClient(client);
                break;
            case MessageTypeSemanticSpaceCreateRequest: {
                std::string SSName;
                std::vector<std::string> geoNames;
                RETURN_NOT_OK(ReadCreateSemanticSpaceRequest(input, SSName, geoNames));
                size_t spaceID = createSemanticSpace(SSName, geoNames, client);             
                HANDLE_SIGPIPE(SendCreateSemanticSpaceReply(client->fd, std::to_string(spaceID)), client->fd);
         
            } break;
            case MessageTypeStorageSpaceCreateRequest: {
                std::string kind;
                std::string spaceID;
                StoreTemplate storeInfo;
                RETURN_NOT_OK(ReadCreateStorageSpaceRequest(input, storeInfo.SSName, storeInfo.spaceSize,
                                                spaceID, kind, storeInfo.writable, storeInfo.connConf));
                size_t storageID = createStorageSpace(spaceID, storeInfo, kind, client);
                HANDLE_SIGPIPE(SendCreateStorageSpaceReply(client->fd, std::to_string(storageID)), client->fd);
            } break;
            case MessageTypeDataImportFromLocalReply: {
                std::string semanticSpaceName;
                std::string storageSpaceName;
                std::string dirPath;
                RETURN_NOT_OK(ReadCreateContentIndexRequest(input, semanticSpaceName, storageSpaceName, dirPath));
                createContentIndex(semanticSpaceName, storageSpaceName, dirPath);
                HANDLE_SIGPIPE(SendCreateContentIndexReply(client->fd, 1), client->fd);
            } break;
            case MessageTypeDataSearchReply: {
                std::vector<std::string> geoNames;
                std::vector<std::string> times;
                std::vector<std::string> varNames;
                ContentID cntID;
                std::string groupName;

                RETURN_NOT_OK(ReadSearchContentIndexRequest(input, geoNames, times, varNames, groupName));
                searchContentIndex(geoNames, times, varNames, cntID, groupName);
                HANDLE_SIGPIPE(SendSearchContentIndexReply(client->fd, cntID.getSpaceID(), cntID.getTimeID(), cntID.getVarID()), client->fd);

            } break;
            default:
                ARROW_CHECK(0);
        }
        return Status::OK();
    }

    void MetaService::runServer() {
        impl_->getLoop()->run();
    }

    bool MetaService::createContentIndexInternal(std::string spaceID, size_t storageID, std::string dirName) {

        auto semanticManger = impl_->getSemanticManager();
        auto storageManager = impl_->getStorageManager();

        // generate time index 
        auto adaptor = storageManager->getAdaptor(storageID);
        semanticManger->createTimeIndex(adaptor, spaceID, dirName);
        semanticManger->createVarIndex(adaptor, spaceID, dirName);
        return true;

    }



}