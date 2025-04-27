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
            std::shared_ptr<SemanticSpaceManager> semanticManager_;

            // storage space manager
            std::shared_ptr<StorageSpaceManager> storageManager_;

            // metadata index
            std::unordered_map<ContentID, ContentDesc, ContentIDHasher> metaIndex_;

            
        public:
            // databox store client
            std::shared_ptr<DataBoxClient> dbClient_;

            Impl(std::shared_ptr<EventLoop> loop) {
                loop_ = std::move(loop);
                semanticManager_ = std::make_shared<SemanticSpaceManager>();
                storageManager_ = std::make_shared<StorageSpaceManager>();
                dbClient_ = DataBoxClient::createClient();
               
            }

            std::shared_ptr<DataBoxClient> getDBClient() {
                return this->dbClient_;
            }

            std::shared_ptr<EventLoop> getLoop() {
                return this->loop_;
            }

            MetaServiceInfo& getMetaServiceInfo() {
                return this->metaInfo_;
            }

            std::unordered_map<std::string, SemanticSpaceEntry*>& getSemanticSpaceInfo() {
                return this->metaInfo_.semanticSpaceInfo_;
            }

            std::unordered_map<std::string, StorageSpaceEntry*>& getStorageSpaceInfo() {
                return this->metaInfo_.storageSpaceInfo_;
            }

            std::vector<uint8_t> & getInputBuffer() {
                return this->inputBuffer;
            }

            std::shared_ptr<SemanticSpaceManager> getSemanticManager() {
                return this->semanticManager_;
            }

            std::shared_ptr<StorageSpaceManager> getStorageManager() {
                return this->storageManager_;
            }

            void setMetaIndex(std::unordered_map<ContentID, ContentDesc, ContentIDHasher> metaIndex) {
                this->metaIndex_ = metaIndex;
            }

            bool getContentDesc(ContentID &cntID, ContentDesc &cntDesc) {
                auto ret = metaIndex_.find(cntID);
                if(ret != metaIndex_.end()) {
                   cntDesc = ret->second;
                   return true;
                }
                return false;
            }

            void addMetaIndex(ContentID &cntID, ContentDesc &cntDesc) {
                metaIndex_[cntID] = cntDesc;
            }

            void insertSemenaticSpaceEntry(std::string spaceName, SemanticSpaceEntry* entry) {
                this->metaInfo_.semanticSpaceInfo_.insert({spaceName, entry});
            }

            void insertStorageSpaceEntry(std::string spaceName, StorageSpaceEntry* entry) {
                this->metaInfo_.storageSpaceInfo_.insert({spaceName, entry});
            }
            
    };

    MetaService::~MetaService() {

    }
    
    
    std::shared_ptr<MetaService> MetaService::createMetaService(std::shared_ptr<EventLoop> loop) {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(loop);
        std::shared_ptr<MetaService> service(new MetaService(impl)); 
        return service;
    }

    MetaService::MetaService(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {
        init();
    }

    void MetaService::init() {
        auto ret = initDataSource();
        if(!ret) {
            ARROW_LOG(DEBUG) << "Cannot Init the data source.";
        }
    }

    SemanticSpace* MetaService::createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames,  MetaClient* client) {
        
        auto SemanticSpaceInfo = impl_->getSemanticSpaceInfo();
        if(SemanticSpaceInfo.count(SSName) != 0) {
           return SemanticSpaceInfo[SSName]->space;
        }

        auto manager = impl_->getSemanticManager();
        std::string spaceID = manager->createSemanticSpace(SSName, geoNames);

        if(spaceID != "" ) {
            SemanticSpace* space = manager->getSpaceByID(spaceID);
            SemanticSpaceEntry* entry = new SemanticSpaceEntry;
            entry->semanticSpaceName = SSName;
            entry->space = space;
            impl_->insertSemenaticSpaceEntry(SSName, entry);
            addClientToSemanticSpaceEntry(entry, client);
            return space;
        }
        return nullptr;
    }

    SemanticSpace* MetaService::loadSemanticSpace(std::string SSName) {
        auto SemanticSpaceInfo = impl_->getSemanticSpaceInfo();
        if(SemanticSpaceInfo.count(SSName) != 0) {
           return SemanticSpaceInfo[SSName]->space;
        } 
        return nullptr;
    }


    StorageSpace* MetaService::createStorageSpace(StoreTemplate &storeInfo, MetaClient* client) {
        std::string SSName = storeInfo.SSName;
        auto StorageSpaceInfo = impl_->getStorageSpaceInfo();
        if(StorageSpaceInfo.count(SSName) != 0) {
           return StorageSpaceInfo[SSName]->space;
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
            return space;
        }
        return nullptr;
    }

    StorageSpace*  MetaService::loadStorageSpace(std::string SSName) {
        auto StorageSpaceInfo = impl_->getStorageSpaceInfo();
        if(StorageSpaceInfo.count(SSName) != 0) {
           return StorageSpaceInfo[SSName]->space;
        } 
        return nullptr;
    }

    bool MetaService::createContentIndex(std::string SemanticSpaceName, std::string StoreSpaceName, std::string dirName) {
        auto SemanticSpaceInfo = impl_->getSemanticSpaceInfo();
        if(SemanticSpaceInfo.count(SemanticSpaceName) == 0) {
            ARROW_LOG(DEBUG) << "Cannot Find Target Semantic Space Name.";
            return false;
        }
        std::string spaceID = SemanticSpaceInfo[SemanticSpaceName]->space->getCompleteSpaceID();
        auto StorageSpaceInfo = impl_->getStorageSpaceInfo();
        if(StorageSpaceInfo.count(StoreSpaceName) == 0) {
            ARROW_LOG(DEBUG) << "Cannot Find Target Storage Space Name.";
            return false;
        }
        size_t storageID = StorageSpaceInfo[StoreSpaceName]->space->spaceID;
        return createContentIndexInternal(spaceID, storageID, dirName);
    }

    bool MetaService::searchDataBox(std::string SSName, std::vector<std::string> &times, std::vector<std::string> &varNames,
                            std::vector<FilePathList> &fileList, std::vector<size_t> &dbIDs) {
     
        
        std::vector<ContentID> cntIDs;
        std::vector<ContentDesc> cntDescs;
        std::vector<StoreDesc> storeDescs;
        auto ret = searchData(SSName, times, varNames, fileList, cntIDs, cntDescs, storeDescs, false);

        if(ret == false) {
            return false;
        }

        // start to create box
        impl_->dbClient_->connect("/tmp/store", "");
        for(int i = 0; i < cntIDs.size(); i++) {
            
            // todo: 这里可以加一个缓存
            DBMeta dbMeta;
            auto ret = impl_->dbClient_->createDB(cntIDs[i], cntDescs[i], storeDescs[i], fileList[i], dbMeta);
            if(ret.ok()) {
                ARROW_LOG(INFO) <<  "databox create success, databox ID is " << dbMeta.id;
                dbIDs.push_back(dbMeta.id);
            }     
        }
        return true;
    }

    bool MetaService::searchDataFile(std::string SSName, std::vector<std::string> &times, 
        std::vector<std::string> &varNames, std::vector<FilePathList> &fileList) {
        std::vector<ContentID> cntIDs;
        std::vector<ContentDesc> cntDescs;
        std::vector<StoreDesc> storeDescs;
        return searchData(SSName, times, varNames, fileList, cntIDs, cntDescs, storeDescs);
    }

    void MetaService::connectToDataBoxService() {
        impl_->dbClient_ = DataBoxClient::createClient();
        auto ret = impl_->dbClient_->connect("/tmp/store", "");
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "db client connect success! \n";
        }
    }

    void MetaService::disconnectToDataBoxService() {
        auto ret = impl_->dbClient_->disconnect();
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "db client disconnect success! \n";
        }
    }



    bool MetaService::searchData(std::string SSName, std::vector<std::string> &times, 
        std::vector<std::string> &varNames, std::vector<FilePathList> &fileList,
        std::vector<ContentID> &cntIDs, std::vector<ContentDesc> &cntDescs, std::vector<StoreDesc> &storeDescs, bool isFile) {
        
        // step1: get semantic sapce by semantic name
        auto semanticManager = impl_->getSemanticManager();  
        ContentMeta* metaManager = semanticManager->getContentMeta(); 
        SemanticSpace* space = semanticManager->getSpaceByName(SSName);
        if(!space) {
            return false;
        }

        for(auto databox : space->databoxsIndex) {
            // step2: determine if the corresponding variable exists in the variable descriptor
            if(metaManager->haveVar(databox.second.vlDesc, varNames) == false) {
                continue;
            }       
            
            // step3: determine if the corresponding time exists in the time descriptor
            std::vector<time_t> filteredTimes;
             if(metaManager->haveTime(databox.second.tsDesc, times, filteredTimes) == false) {
                continue;
            }        
            
            
            StorageID storageID = databox.first.getBestStoID();
            Adaptor* adaptor = impl_->getStorageManager()->getSpaceByID(std::stoi(storageID.getSpaceID()))->getAdaptor(storageID);
            if(!adaptor) {
                continue;
            }

            FilePathList* originList = adaptor->getFilePathList();
            FilePathList pathList(originList);
            for(auto time: filteredTimes) {
                std::string fileName;
                time_to_string(time, fileName, originList->fileType);
                if(originList->fileType == "nc") {
                    pathList.fileNames.push_back("ldasin." + fileName + ".nc");
                } else if(originList->fileType == "HDF") {
                    pathList.fileNames.push_back("FY3D_MERSI_GBAL_L2_CLM_MLT_GLL_" + fileName + "_POAD_5000M_MS.HDF");
                } else {
                    pathList.fileNames.push_back(fileName);
                }
            }

            if(!isFile) {
                // step4: save databoxx info
                StoreDesc storeDesc;
                storeDesc.setConnectConfig(adaptor->connConfig);
                storeDesc.setStoreKindbyTypeID(storageID.getTypeID());
                cntIDs.push_back(databox.first);
                cntDescs.push_back(databox.second);
                storeDescs.push_back(storeDesc);

            }
            
            // step5: save baisc info
            fileList.push_back(pathList);  
        }
        return true;
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

        auto loop = impl_->getLoop();
        loop->add_file_event(clientFd, kEventLoopRead, [this, client](int events) {
            processMessage(client);
        });
    }
   
    void MetaService::disconnectClient(MetaClient* client) {
        assert(client != nullptr);
        assert(client->fd > 0);
        
        impl_->getLoop()->remove_file_event(client->fd);
        close(client->fd);

        for(const auto& entry : impl_->getSemanticSpaceInfo()) {
            removeClientFromSemanticSpaceEntry(entry.second, client);
        }

        for(const auto& entry : impl_->getStorageSpaceInfo()) {
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
                SemanticSpace* space = createSemanticSpace(SSName, geoNames, client);             
                HANDLE_SIGPIPE(SendCreateSemanticSpaceReply(client->fd, space), client->fd);
            } break;
            case MessageTypeSemanticSpaceLoadRequest: {
                std::string SSName;
                RETURN_NOT_OK(ReadLoadSemanticSpaceRequest(input, SSName));
                SemanticSpace* space = loadSemanticSpace(SSName);
                HANDLE_SIGPIPE(SendLoadSemanticSpaceReply(client->fd, space), client->fd);
            } break;
            case MessageTypeStorageSpaceLoadRequest: {
                std::string SSName;
                RETURN_NOT_OK(ReadLoadStorageSpaceRequest(input, SSName));
                StorageSpace* space = loadStorageSpace(SSName);
                HANDLE_SIGPIPE(SendLoadStorageSpaceReply(client->fd, space), client->fd);
            } break;
            case MessageTypeStorageSpaceCreateRequest: {
                std::string kind;
                std::string spaceID;
                StoreTemplate storeInfo;
                RETURN_NOT_OK(ReadCreateStorageSpaceRequest(input, storeInfo.SSName, storeInfo.spaceSize,
                                                spaceID, kind, storeInfo.writable, storeInfo.connConf));
                storeInfo.setStoreKind(kind);
                auto space = createStorageSpace(storeInfo, client);
                HANDLE_SIGPIPE(SendCreateStorageSpaceReply(client->fd, space), client->fd);
            } break;
            case MessageTypeTimeIndexRequest: {
                auto timeIndex = getTimeIndex();
                HANDLE_SIGPIPE(SendTimeIndexReply(client->fd, timeIndex), client->fd);
            } break;
            case MessageTypeVarIndexRequest: {
                auto varIndex = getVarIndex();
                HANDLE_SIGPIPE(SendVarIndexReply(client->fd, varIndex), client->fd);
            } break;
            case MessageTypeDataImportFromLocalRequest: {
                std::string semanticSpaceName;
                std::string storageSpaceName;
                std::string dirPath;
                RETURN_NOT_OK(ReadCreateContentIndexRequest(input, semanticSpaceName, storageSpaceName, dirPath));
                auto ret = createContentIndex(semanticSpaceName, storageSpaceName, dirPath);
                HANDLE_SIGPIPE(SendCreateContentIndexReply(client->fd, ret), client->fd);
            } break;
            case MessageTypeDataBoxSearchRequest: {
                size_t databoxID;
                std::string SSName;
                std::vector<std::string> times;
                std::vector<std::string> varNames;
                std::vector<FilePathList> fileList;
                std::vector<size_t> dbIDs;
                RETURN_NOT_OK(ReadSearchDataBoxRequest(input, SSName, times, varNames));
                auto ret = searchDataBox(SSName, times, varNames, fileList,  dbIDs);
                HANDLE_SIGPIPE(SendSearchDataBoxReply(client->fd, dbIDs, fileList), client->fd);
            } break;
            case MessageTypeDataFileSearchRequest: {
                std::string SSName;
                std::vector<std::string> times;
                std::vector<std::string> varNames;
                std::vector<FilePathList> fileList;
                RETURN_NOT_OK(ReadSearchDataFileRequest(input, SSName, times, varNames));
                searchDataFile(SSName, times, varNames, fileList);
                // search data file by semantic name
                HANDLE_SIGPIPE(SendSearchDataFileReply(client->fd, fileList), client->fd);
            } break;
            case MessageTypeBindDataSourceRequest: { 
                std::string SSName;
                StorageID storeID;
                RETURN_NOT_OK(ReadBindDataSourceRequest(input, SSName, storeID));
                auto ret = bindDataSource(SSName, storeID);
                HANDLE_SIGPIPE(SendBindDataSourceReply(client->fd, ret), client->fd);
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
        
        StorageID storeID;
        if(storageManager->createStoreTreeIndex(storageID, dirName, storeID)) {
            Adaptor* adaptor = storageManager->getSpaceByID(storageID)->getAdaptor(storeID);
            return semanticManger->createDataBoxIndex(spaceID, storeID, adaptor);
        }  
    }

    bool MetaService::initDataSource() {
        // step1: create storage space for each backend storage system
        auto storageManager = impl_->getStorageManager();
        auto semanticManager = impl_->getSemanticManager();
        StoreTemplate temp;
        temp.SSName = "cephfs";
        temp.connConf.rootPath = "/home/snivyer/ceph_mount/data";
        temp.kind = StoreSpaceKind::Local;

        auto space = createStorageSpace(temp, nullptr);
        std::vector<std::string> sourcePath = getDirectoriesWithFiles(temp.connConf.rootPath);

        // each path should be instaced one adaptor
        for(auto path : sourcePath) {
            StorageID storageID;
            storageManager->createStoreTreeIndex(space->spaceID, path, storageID);
            Adaptor* adaptor = storageManager->getSpaceByID(space->spaceID)->getAdaptor(storageID);
            semanticManager->createDataSourceIndex(storageID, adaptor);
        }
        return true;
    }

    bool MetaService::bindDataSource(std::string SSName, StorageID &storeID) {
        auto storageManager = impl_->getStorageManager();
        StorageSpace* space = storageManager->getSpaceByName(SSName);
        if(!space) {
            return false;
        }

        size_t sourceStoreID = std::stoi(storeID.getSpaceID());
        auto sourceSpace = storageManager->getSpaceByID(sourceStoreID);
        if(!sourceSpace) {
            return false;
        }

        Adaptor* adaptor = sourceSpace->getAdaptor(storeID);
        if(!adaptor) {
            return false;
        }
        space->addAdaptor(storeID, adaptor);
        auto semanticManger = impl_->getSemanticManager();
        std::string spaceID = semanticManger->getSpaceByName(SSName)->getCompleteSpaceID();
        return semanticManger->createDataBoxIndex(spaceID, storeID, adaptor);
    }

    TimeIndex* MetaService::getTimeIndex() {
        auto spaceManager = impl_->getSemanticManager();
        return spaceManager->getTimeIndex();
    }

    VarIndex* MetaService::getVarIndex() {
        auto spaceManager = impl_->getSemanticManager();
        return spaceManager->getVarIndex();
    }
}