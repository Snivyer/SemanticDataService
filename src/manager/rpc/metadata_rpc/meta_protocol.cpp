#include "manager/rpc/metadata_rpc/meta_protocol.h"


namespace SDS {

    template <typename Message>
    Status messageSend(int sock, int64_t message_type, flatbuffers::FlatBufferBuilder* fbb,
                        const Message& message) {
        fbb->Finish(message);
        return WriteMessage(sock, message_type, fbb->GetSize(), fbb->GetBufferPointer());
    }

    Status SendMetaConnectRequest(int sock) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateMetaClientConnectRequest(fbb, sock);
        return messageSend(sock, MessageTypeMetaConnectRequest, &fbb, message);
    }

    Status ReadMetaConnectRequest(uint8_t* data) {
        return Status::OK();
    }

    Status SendMetaConnectReply(int sock, int64_t welcome) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateMetaClientConnectReply(fbb, welcome);
        return messageSend(sock, MessageTypeMetaConnectReply, &fbb, message);
    }


    Status ReadMetaConnectReply(uint8_t* data, int64_t* welcome) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<MetaClientConnectReply>(data);
        *welcome = message->welcome();
        return Status::OK();
    }

    Status ReadErrorReply(uint8_t* data, bool &status) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StatusReply>(data);
        status= message->status();
        return Status::OK();
    }


    Status SendCreateSemanticSpaceRequest(int sock, std::string spaceName, std::vector<std::string> geoNames) {
        ARROW_LOG(INFO) <<  "Send create semantic space request, spaceName:" << spaceName;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceNamef = fbb.CreateString(spaceName);

        std::vector<flatbuffers::Offset<flatbuffers::String>> geoNamesf;
        for(auto item: geoNames) {
            auto geoNamef = fbb.CreateString(item);
            geoNamesf.push_back(geoNamef);
        }
        
        auto geoNameVector = fbb.CreateVector(geoNamesf);
        auto message = CreateSemanticSpaceCreateRequest(fbb, spaceNamef, geoNameVector);
        return messageSend(sock, MessageTypeSemanticSpaceCreateRequest, &fbb, message);
    }

     Status ReadCreateSemanticSpaceRequest(uint8_t* data, std::string &spaceName, std::vector<std::string> &geoNames) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<SemanticSpaceCreateRequest>(data);
        spaceName = message->space_name()->str();
        auto geoNamesList = message->geo_names();

        for(int i = 0; i < geoNamesList->size(); i++) {
            geoNames.push_back(geoNamesList->Get(i)->str());
        }
        return Status::OK();
    }

    Status SendCreateSemanticSpaceReply(int sock, SemanticSpace* space) {
        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(std::to_string(space->spaceID));
        auto ssNamef = fbb.CreateString(space->SSName);
        auto pssIDf = fbb.CreateString(space->PSSID);
        auto adcodef = fbb.CreateString(space->ssDesc.adCode);
        auto geoNamef = fbb.CreateString(space->ssDesc.geoName);

        std::vector<double> geo_logitude;
        std::vector<double> geo_latitude;
        for(auto item : space->ssDesc.geoPerimeter) {
            geo_logitude.push_back(item.logitude);
            geo_latitude.push_back(item.latitude);
        }
        auto geo_logitude_vector = fbb.CreateVector(geo_logitude);
        auto geo_latitude_vector = fbb.CreateVector(geo_latitude);

        auto message = CreateSemanticSpaceCreateReply(fbb, spaceIDf, ssNamef, pssIDf, space->childrenNum, 
                                                        space->createT, space->databoxNum, geoNamef,
                                                        adcodef, space->ssDesc.geoCentral.logitude, space->ssDesc.geoCentral.latitude,
                                                        geo_logitude_vector, geo_latitude_vector);
        return messageSend(sock, MessageTypeSemanticSpaceCreateReply, &fbb, message);
    }

    Status ReadCreateSemanticSpaceReply(uint8_t* data, SemanticSpace& space) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<SemanticSpaceCreateReply>(data);
        space.spaceID = std::atoi(message->space_id()->str().c_str());
        space.SSName = message->ssname()->str();
        space.PSSID = message->psss_id()->str();
        space.childrenNum = message->children_num();
        space.createT = message->create_time();
        space.databoxNum = message->databox_num();
        space.ssDesc.geoName = message->geo_names()->str();
        space.ssDesc.adCode = message->adcode()->str();
        space.ssDesc.geoCentral.logitude = message->logitude();
        space.ssDesc.geoCentral.latitude = message->latitude();

        auto geo_logitude_vector = message->perimeter_logitude();
        auto geo_latitude_vector = message->perimeter_latitude();
        for(int i = 0; i < geo_logitude_vector->size(); i++) {
            GeoCoordinate geo;
            geo.logitude = geo_logitude_vector->Get(i);
            geo.latitude = geo_latitude_vector->Get(i);
            space.ssDesc.geoPerimeter.push_back(geo);
        }
        return Status::OK();   
    }

    Status SendLoadSemanticSpaceRequest(int sock, std::string spaceName) {
        ARROW_LOG(INFO) <<  "Send load semantic space request, spaceName:" << spaceName;
        flatbuffers::FlatBufferBuilder fbb;
        auto spaceNamef = fbb.CreateString(spaceName);
        auto message = CreateSemanticSpaceLoadRequest(fbb, spaceNamef);
        return messageSend(sock, MessageTypeSemanticSpaceLoadRequest, &fbb, message);
    }

    Status ReadLoadSemanticSpaceRequest(uint8_t* data, std::string &spaceName) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<SemanticSpaceLoadRequest>(data);
        spaceName = message->space_name()->str();
        return Status::OK();
    }

    Status SendLoadSemanticSpaceReply(int sock, SemanticSpace* space) {
        flatbuffers::FlatBufferBuilder fbb1;
        bool status = true;
        if(space == nullptr) {
            ARROW_LOG(INFO) <<  "cannot find object semantic space...";
            bool status = false;
            auto message = CreateStatusReply(fbb1, status);
            return messageSend(sock, MessageTypeErrorReply, &fbb1, message);
        }

        auto message = CreateStatusReply(fbb1, status);
        messageSend(sock, MessageTypeErrorReply, &fbb1, message);

        flatbuffers::FlatBufferBuilder fbb;
        std::vector<ContentID> cntIDVec;
        std::vector<ContentDesc> cntDescVec;
        for(auto item: space->databoxsIndex) {
            cntIDVec.push_back(item.first);
            cntDescVec.push_back(item.second);
        }

        std::vector<flatbuffers::Offset<ContentIDRequest>> cntIDfVec;
        std::vector<flatbuffers::Offset<ContentDescRequest>> cntDescfVec;

        // Serialize Space info
        auto spaceIDf = fbb.CreateString(std::to_string(space->spaceID));
        auto ssNamef = fbb.CreateString(space->SSName);
        auto pssIDf = fbb.CreateString(space->PSSID);
        auto adcodef = fbb.CreateString(space->ssDesc.adCode);
        auto geoNamef = fbb.CreateString(space->ssDesc.geoName);

        std::vector<double> geo_logitude;
        std::vector<double> geo_latitude;
        for(auto item : space->ssDesc.geoPerimeter) {
            geo_logitude.push_back(item.logitude);
            geo_latitude.push_back(item.latitude);
        }
        auto geo_logitude_vector = fbb.CreateVector(geo_logitude);
        auto geo_latitude_vector = fbb.CreateVector(geo_latitude);

        auto spaceInfof = CreateSemanticSpaceCreateReply(fbb, spaceIDf, ssNamef, pssIDf, space->childrenNum, 
                                                        space->createT, space->databoxNum, geoNamef,
                                                        adcodef, space->ssDesc.geoCentral.logitude, space->ssDesc.geoCentral.latitude,
                                                        geo_logitude_vector, geo_latitude_vector);

        // Serialize Content ID
        for(int i = 0; i < cntIDVec.size(); i++) {
            auto cntIDf = GetContentID(fbb, cntIDVec[i]);
            cntIDfVec.push_back(cntIDf);
        }
        auto cntIDfVecf = fbb.CreateVector(cntIDfVec);

        // Serialize Content Decription
        for(int i = 0; i < cntDescVec.size(); i++) {
            auto cntDescf = GetContentDesc(fbb, cntDescVec[i]);
            cntDescfVec.push_back(cntDescf);
        }
        auto cntDescfVecf = fbb.CreateVector(cntDescfVec);
        auto message2 = CreateSemanticSpaceLoadReply(fbb, spaceInfof, cntIDfVecf, cntDescfVecf);
        return messageSend(sock, MessageTypeSemanticSpaceLoadReply, &fbb, message2);
    }

    Status ReadLoadSemanticSpaceReply(uint8_t* data, SemanticSpace &space) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<SemanticSpaceLoadReply>(data);
        auto cntIDVecf =  message->cnt_ids();
        auto cntDescVecf = message->cnt_descs();
        auto spaceInfo = message->semanticspace();
        
        space.spaceID = std::atoi(spaceInfo->space_id()->str().c_str());
        space.SSName = spaceInfo->ssname()->str();
        space.PSSID = spaceInfo->psss_id()->str();
        space.childrenNum = spaceInfo->children_num();
        space.createT = spaceInfo->create_time();
        space.databoxNum = spaceInfo->databox_num();
        space.ssDesc.geoName = spaceInfo->geo_names()->str();
        space.ssDesc.adCode = spaceInfo->adcode()->str();
        space.ssDesc.geoCentral.logitude = spaceInfo->logitude();
        space.ssDesc.geoCentral.latitude = spaceInfo->latitude();

        auto geo_logitude_vector = spaceInfo->perimeter_logitude();
        auto geo_latitude_vector = spaceInfo->perimeter_latitude();
        for(int i = 0; i < geo_logitude_vector->size(); i++) {
            GeoCoordinate geo;
            geo.logitude = geo_logitude_vector->Get(i);
            geo.latitude = geo_latitude_vector->Get(i);
            space.ssDesc.geoPerimeter.push_back(geo);
        }
    
        for(int i = 0; i < cntIDVecf->size(); i++) {
            // Deserialize Content ID
            ContentID cntID;
            SetContentID(cntIDVecf->Get(i), cntID);

            ContentDesc cntDesc;
            SetContentDesc(cntDescVecf->Get(i), cntDesc);
            space.databoxsIndex.insert({cntID, cntDesc});
        } 
        return Status::OK();
    }

    Status SendCreateStorageSpaceRequest(int sock, std::string SSName, size_t capacitySize, std::string spaceID, std::string storageKind,
                                         bool writable, ConnectConfig &config) {
        
        ARROW_LOG(INFO) <<  "Send create storage space request, spaceName:" << SSName;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceNamef = fbb.CreateString(SSName);
        auto spaceIDf = fbb.CreateString(spaceID);
        auto storageKindf = fbb.CreateString(storageKind);
        auto userNamef = fbb.CreateString(config.userName);
        auto confFilef = fbb.CreateString(config.confFile);
        auto rootPathf = fbb.CreateString(config.rootPath);
        auto message = CreateStorageSpaceCreateRequest(fbb, spaceNamef, writable, storageKindf,
                                                        capacitySize, spaceIDf, userNamef, confFilef,
                                                        rootPathf);
        return messageSend(sock, MessageTypeStorageSpaceCreateRequest, &fbb, message);
    }
    
    
    Status ReadCreateStorageSpaceRequest(uint8_t* data, std::string& SSName, size_t &capacitySize, std::string& spaceID, std::string &storageKind,
                                         bool &writable, ConnectConfig &config) {

        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceCreateRequest>(data);
        SSName =  message->space_name()->str();
        capacitySize = message->space_size();
        spaceID = message->space_id()->str();
        storageKind = message->kind()->str();
        writable = message->writable();
        config.confFile = message->conf_file()->str();
        config.userName = message->user_name()->str();
        config.rootPath = message->root_path()->str();
        return Status::OK();
    }

    Status SendCreateStorageSpaceReply(int sock, StorageSpace *space){
        flatbuffers::FlatBufferBuilder fbb;
        auto storageIDf = fbb.CreateString(std::to_string(space->spaceID));
        auto ssNamef = fbb.CreateString(space->stoMeta.SSName);
        auto rootPathf = fbb.CreateString(space->stoMeta.conConf.rootPath);
        std::string kind = space->stoMeta.getStoreKind();
        auto kindf = fbb.CreateString(kind);
        auto message = CreateStorageSpaceCreateReply(fbb, storageIDf, ssNamef, space->stoMeta.writable,
                                                    space->stoMeta.size, space->stoMeta.capacity, kindf, rootPathf);
        return messageSend(sock, MessageTypeStorageSpaceCreateReply, &fbb, message);
    }

    Status ReadCreateStorageSpaceReply(uint8_t* data, StorageSpace& space) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceCreateReply>(data);
        space.spaceID = std::atoi(message->storage_id()->str().c_str());
        space.stoMeta.SSName = message->ssname()->str();
        space.stoMeta.writable = message->writable();
        space.stoMeta.size = message->size();
        space.stoMeta.capacity = message->capacity();
        space.stoMeta.conConf.rootPath = message->root_path()->str();
        space.stoMeta.setStoreKind(message->kind()->str());
        return Status::OK();   
    }

    Status SendLoadStorageSpaceRequest(int sock, std::string spaceName) {
        ARROW_LOG(INFO) <<  "Send load storage space request, storageName:" << spaceName;
        flatbuffers::FlatBufferBuilder fbb;
        auto spaceNamef = fbb.CreateString(spaceName);
        auto message = CreateStorageSpaceLoadRequest(fbb, spaceNamef);
        return messageSend(sock, MessageTypeStorageSpaceLoadRequest, &fbb, message);
    }


    Status ReadLoadStorageSpaceRequest(uint8_t* data, std::string &spaceName) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceLoadRequest>(data);
        spaceName = message->space_name()->str();
        return Status::OK();
    }


    Status SendLoadStorageSpaceReply(int sock, StorageSpace* space) {
        flatbuffers::FlatBufferBuilder fbb1;
        bool status = true;
        if(space == nullptr) {
            ARROW_LOG(INFO) <<  "cannot find object storage space...";
            bool status = false;
            auto message = CreateStatusReply(fbb1, status);
            return messageSend(sock, MessageTypeErrorReply, &fbb1, message);
        }

        auto message = CreateStatusReply(fbb1, status);
        messageSend(sock, MessageTypeErrorReply, &fbb1, message);
        
        
        flatbuffers::FlatBufferBuilder fbb;
        auto storageIDf = fbb.CreateString(std::to_string(space->spaceID));
        auto ssNamef = fbb.CreateString(space->stoMeta.SSName);
        auto rootPathf = fbb.CreateString(space->stoMeta.conConf.rootPath);
        std::string kind = space->stoMeta.getStoreKind();
        auto kindf = fbb.CreateString(kind);
        auto storagespacef = CreateStorageSpaceCreateReply(fbb, storageIDf, ssNamef, space->stoMeta.writable,
                                                    space->stoMeta.size, space->stoMeta.capacity, kindf, rootPathf);
        
        // Serialize storageID and filePathList
        std::vector<flatbuffers::Offset<StorageIDRequest>> stoIDfVec;
        std::vector<flatbuffers::Offset<FilePathListRequest>> filePathListVec;

        for(auto item: space->adaptorIndex) {
            auto spaceIDf = fbb.CreateString(item.first.getSpaceID());
            auto typeIDf = fbb.CreateString(item.first.getTypeID());
            auto siteIDf = fbb.CreateString(item.first.getSiteID());
            auto storeIDf = CreateStorageIDRequest(fbb, spaceIDf, typeIDf, siteIDf);
            stoIDfVec.push_back(storeIDf);

            auto filePathListf = GetFilePathList(fbb, item.second->pathList);
            filePathListVec.push_back(filePathListf);
        }

        auto stoIDfVecf = fbb.CreateVector(stoIDfVec);
        auto filePathListVecf = fbb.CreateVector(filePathListVec);
        auto message2 = CreateStorageSpaceLoadReply(fbb, storagespacef, stoIDfVecf, filePathListVecf);
        return messageSend(sock, MessageTypeStorageSpaceLoadReply, &fbb, message2);

    }


    Status ReadLoadStorageSpaceReply(uint8_t* data, StorageSpace &space) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceLoadReply>(data);
        space.spaceID = std::atoi(message->storagespace()->storage_id()->str().c_str());
        space.stoMeta.SSName = message->storagespace()->ssname()->str();
        space.stoMeta.writable = message->storagespace()->writable();
        space.stoMeta.size = message->storagespace()->size();
        space.stoMeta.capacity = message->storagespace()->capacity();
        space.stoMeta.conConf.rootPath = message->storagespace()->root_path()->str();
        space.stoMeta.setStoreKind(message->storagespace()->kind()->str());

        auto storeIDs = message->sto_ids();
        auto pathList = message->file_path_list();

        for(int i = 0; i < storeIDs->size(); i++) {
            StorageID stoID;
            stoID.setSpaceID(message->sto_ids()->Get(i)->space_id()->c_str());
            stoID.setTypeID(message->sto_ids()->Get(i)->type_id()->c_str());
            stoID.setSiteID(message->sto_ids()->Get(i)->site_id()->c_str());

            FilePathList* pathList = new FilePathList();
            SetFilePathList(message->file_path_list()->Get(i), pathList);
            Adaptor* adaptor = new Adaptor(space.stoMeta.conConf, pathList);
            space.adaptorIndex.insert({stoID, adaptor});
        }
        return Status::OK();
    }


    
    Status SendCreateContentIndexRequest(int sock, std::string semanticSpaceName, std::string storageSpaceName, std::string dirPath) {
 
        ARROW_LOG(INFO) <<  "Send content index create request, dirPath:" << dirPath;
        flatbuffers::FlatBufferBuilder fbb;
        auto semanticSpaceNamef = fbb.CreateString(semanticSpaceName);
        auto storageSpaceNamef = fbb.CreateString(storageSpaceName);
        auto dirPathf = fbb.CreateString(dirPath);
        auto message = CreateContentIndexCreateRequst(fbb, semanticSpaceNamef, storageSpaceNamef, dirPathf);
        
        return messageSend(sock, MessageTypeDataImportFromLocalRequest, &fbb, message);
    }

    Status ReadCreateContentIndexRequest(uint8_t* data, std::string &semanticSpaceName, std::string &storageSpaceName, std::string &dirPath) {
        
        DCHECK(data);
        auto message = flatbuffers::GetRoot<ContentIndexCreateRequst>(data);
        semanticSpaceName = message->semantic_space_name()->c_str();
        storageSpaceName = message->storage_space_name()->c_str();
        dirPath = message->dir_path()->c_str();
        return Status::OK();
    }

    Status SendCreateContentIndexReply(int sock, bool result) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateContentIndexReply(fbb, result);
        return messageSend(sock, MessageTypeDataImportFromLocalReply, &fbb, message);
    }

    Status ReadCreateContentIndexReply(uint8_t* data, bool &result) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<ContentIndexReply>(data);
        result = message->result();
        return Status::OK();
    }

  
   


    Status SendSearchDataBoxRequest(int sock, std::string &SSName, std::vector<std::string> &times, 
                                    std::vector<std::string> &varNames) {
        ARROW_LOG(INFO) <<  "Send data box search request:";
        flatbuffers::FlatBufferBuilder fbb;
        auto SSNamef = fbb.CreateString(SSName);
        std::vector<flatbuffers::Offset<flatbuffers::String>> timesf;
        std::vector<flatbuffers::Offset<flatbuffers::String>> varNamesf;

        for(auto time: times) {
            auto timef = fbb.CreateString(time);
            timesf.push_back(timef);
        }

        for(auto name: varNames) {
            auto namef = fbb.CreateString(name);
            varNamesf.push_back(namef);
        }

        auto timeVector = fbb.CreateVector(timesf);
        auto varNameVector = fbb.CreateVector(varNamesf);
        auto message = CreateDataFileSearchRequest(fbb, SSNamef, timeVector, varNameVector);
        return messageSend(sock, MessageTypeDataBoxSearchRequest, &fbb, message);
      
    }
    
    Status ReadSearchDataBoxRequest(uint8_t* data, std::string &SSName, std::vector<std::string> &times, 
                                        std::vector<std::string> &varNames) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DataFileSearchRequest>(data);
        SSName = message->semantic_name()->str();
        auto timeVector = message->times();
        auto varVector = message->var_names();

        for(int i = 0; i < timeVector->size(); i++) {
            times.push_back(timeVector->Get(i)->str());
        }

        for(int i = 0; i < varVector->size(); i++) {
            varNames.push_back(varVector->Get(i)->str());
        }

        return Status::OK();
    }

    Status SendSearchDataBoxReply(int sock, std::vector<size_t> &dbIDs, std::vector<FilePathList> &filePath) {
        flatbuffers::FlatBufferBuilder fbb;
        auto dbIDsf = fbb.CreateVector(dbIDs);

        std::vector<flatbuffers::Offset<FilePathListRequest>> filePathf;
        for(auto item: filePath) {
            auto filePathListf = GetFilePathList(fbb, &item);
            filePathf.push_back(filePathListf);
        }
        auto filePathVectorf = fbb.CreateVector(filePathf);
        auto  message = CreateDataBoxSearchReply(fbb, dbIDsf, filePathVectorf);
        return messageSend(sock, MessageTypeDataBoxSearchReply, &fbb, message);     
    }

    Status ReadSearchDataBoxReply(uint8_t* data, std::vector<size_t> &dbIDs, std::vector<FilePathList> &filePath) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DataBoxSearchReply>(data);
        auto databoxIDs = message->databox_id();
        for(int i = 0; i < databoxIDs->size(); i++) {
            dbIDs.push_back(databoxIDs->Get(i));
        }

        auto filePathVector = message->file_path_lists();
        for(int i = 0; i < filePathVector->size(); i++) {
            FilePathList pathList;
            SetFilePathList(filePathVector->Get(i), &pathList);
            filePath.push_back(pathList);
        }
        return Status::OK();
    }

    Status SendSearchDataFileRequest(int sock, std::string &SSName, std::vector<std::string> &times,
                                     std::vector<std::string> &varNames) {
        ARROW_LOG(INFO) <<  "Send data file search request:";
        flatbuffers::FlatBufferBuilder fbb;
        auto SSNamef = fbb.CreateString(SSName);
        std::vector<flatbuffers::Offset<flatbuffers::String>> timesf;
        std::vector<flatbuffers::Offset<flatbuffers::String>> varNamesf;

        for(auto time: times) {
            auto timef = fbb.CreateString(time);
            timesf.push_back(timef);
        }

        for(auto name: varNames) {
            auto namef = fbb.CreateString(name);
            varNamesf.push_back(namef);
        }

        auto timeVector = fbb.CreateVector(timesf);
        auto varNameVector = fbb.CreateVector(varNamesf);
        auto message = CreateDataFileSearchRequest(fbb, SSNamef, timeVector, varNameVector);
        return messageSend(sock, MessageTypeDataFileSearchRequest, &fbb, message);
    }


    Status ReadSearchDataFileRequest(uint8_t* data, std::string &SSName, std::vector<std::string> &times, std::vector<std::string> &varNames) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DataFileSearchRequest>(data);
        SSName = message->semantic_name()->str();
        auto timeVector = message->times();
        auto varVector = message->var_names();

        for(int i = 0; i < timeVector->size(); i++) {
            times.push_back(timeVector->Get(i)->str());
        }

        for(int i = 0; i < varVector->size(); i++) {
            varNames.push_back(varVector->Get(i)->str());
        }

        return Status::OK();
    }

    Status SendSearchDataFileReply(int sock, std::vector<FilePathList> &filePath) {
        flatbuffers::FlatBufferBuilder fbb;
        std::vector<flatbuffers::Offset<FilePathListRequest>> filePathf;
        for(auto item: filePath) {
            auto filePathListf = GetFilePathList(fbb, &item);
            filePathf.push_back(filePathListf);
        }
        auto filePathVectorf = fbb.CreateVector(filePathf);
        auto message = CreateDataFileSearchReply(fbb, filePathVectorf);
        return messageSend(sock, MessageTypeDataFileSearchReply, &fbb, message);
    }



    Status ReadSearchDataFileReply(uint8_t* data, std::vector<FilePathList> &filePath) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DataFileSearchReply>(data);
        auto filePathVector = message->file_path_lists();
        for(int i = 0; i < filePathVector->size(); i++) {
            FilePathList pathList;
            SetFilePathList(filePathVector->Get(i), &pathList);
            filePath.push_back(pathList);
        }
        return Status::OK();
    }

    flatbuffers::Offset<StoreSiteRequest> GetSite(flatbuffers::FlatBufferBuilder &fbb, StoreSite *site) {
        auto siteNamef = fbb.CreateString(site->siteName);
        auto siteValf = fbb.CreateString(site->siteVal);
        std::vector<flatbuffers::Offset<StoreSiteRequest>> childSitesVector;
        for(auto childSite: site->subSites) {
            auto childMessage = GetSite(fbb, childSite);
            childSitesVector.push_back(childMessage);
        }
        auto childSitesVectorf = fbb.CreateVector(childSitesVector);

        return CreateStoreSiteRequest(fbb, siteNamef, siteValf, childSitesVectorf);
    }

    Status SetSite(const flatbuffers::Vector<flatbuffers::Offset<StoreSiteRequest>> *siteVectorf, std::vector<StoreSite*> *siteVector) { 
        for(int i = 0; i < siteVectorf->size(); i++) {
            StoreSite* site = new  StoreSite();
            site->siteName = siteVectorf->Get(i)->site_name()->c_str();
            site->siteVal = siteVectorf->Get(i)->site_val()->c_str(); 
            auto childSites = siteVectorf->Get(i)->sub_sites();   
            SetSite(childSites, &(site->subSites));
            siteVector->push_back(site);
        }
        return Status::OK();
    }

    flatbuffers::Offset<ContentIDRequest> GetContentID(flatbuffers::FlatBufferBuilder &fbb, ContentID &cntID) {
        auto spaceIDf = fbb.CreateString(cntID.getSpaceID());
        auto timeIDf = fbb.CreateString(cntID.getTimeID());
        auto varIDf = fbb.CreateString(cntID.getVarID());
        auto cntIDf = CreateContentIDRequest(fbb, spaceIDf, timeIDf, varIDf);
        return cntIDf;
    }

    Status SetContentID(const ContentIDRequest *cntIDf, ContentID &cntID) {
        cntID.setSpaceID(cntIDf->space_id()->c_str());
        cntID.setTimeID(cntIDf->time_id()->c_str());
        cntID.setVarID(cntIDf->var_id()->c_str());
        return Status::OK();
    }

    flatbuffers::Offset<ContentDescRequest> GetContentDesc(flatbuffers::FlatBufferBuilder &fbb, ContentDesc &cntDesc) {
        // Serialize Space Description
        auto geoNamef = fbb.CreateString(cntDesc.ssDesc.geoName);
        auto adcodef = fbb.CreateString(cntDesc.ssDesc.adCode);
        std::vector<double> geo_logitude;
        std::vector<double> geo_latitude;

        for(auto item : cntDesc.ssDesc.geoPerimeter) {
            geo_logitude.push_back(item.logitude);
            geo_latitude.push_back(item.latitude);
        }
        auto geo_logitude_vec = fbb.CreateVector(geo_logitude);
        auto geo_latitude_vec = fbb.CreateVector(geo_latitude);

        auto SSDescf = CreateSSDescRequest(fbb, geoNamef, adcodef, cntDesc.ssDesc.geoCentral.logitude,
            cntDesc.ssDesc.geoCentral.latitude, geo_logitude_vec, geo_latitude_vec);
        
        // Serialize Time Decription
        time_t reportT = std::mktime(&(cntDesc.tsDesc.reportT));
        time_t startT = std::mktime(&(cntDesc.tsDesc.startT));
        time_t endT = std::mktime(&(cntDesc.tsDesc.endT));
        auto tsDescf = CreateTSDescRequest(fbb, reportT, startT, endT,
            cntDesc.tsDesc.interval, cntDesc.tsDesc.count);

        // Serialize Var Description
        auto groupNamef = fbb.CreateString(cntDesc.vlDesc.groupName);
        std::vector<flatbuffers::Offset<AttrRequest>> globalAttrsfVec;
        for(auto item : cntDesc.vlDesc.attrs) {
            auto attrNamef = fbb.CreateString(item.first);
            auto attrValf = fbb.CreateString(item.second);
            auto attrf = CreateAttrRequest(fbb, attrNamef, attrValf);
            globalAttrsfVec.push_back(attrf);
        }
        auto globalAttrsfVecf = fbb.CreateVector(globalAttrsfVec);

        std::vector<flatbuffers::Offset<VarDescRequest>> varDescfVec;
        for(auto item : cntDesc.vlDesc.desc) {
            auto varNamef = fbb.CreateString(item.varName);
            auto varTypef = fbb.CreateString(item.varType);
            auto shapeVecf = fbb.CreateVector(item.shape);
            auto groupPath = fbb.CreateString(item.groupPath);
            std::vector<flatbuffers::Offset<AttrRequest>> attrsfVec;
            for(auto item : cntDesc.vlDesc.attrs) {
                auto attrNamef = fbb.CreateString(item.first);
                auto attrValf = fbb.CreateString(item.second);
                auto attrf = CreateAttrRequest(fbb, attrNamef, attrValf);
                attrsfVec.push_back(attrf);
            }
            auto attrsfVecf = fbb.CreateVector(attrsfVec);
            auto varDescf = CreateVarDescRequest(fbb, varNamef, varTypef, item.varLen,
                                                    item.resRation, shapeVecf, item.ncVarID,
                                                    item.ncGroupID, groupPath, attrsfVecf);
            varDescfVec.push_back(varDescf);
        }
        auto varDescfVecf = fbb.CreateVector(varDescfVec);
        auto vlDescf = CreateVLDescRequest(fbb, groupNamef, cntDesc.vlDesc.groupLen, varDescfVecf, globalAttrsfVecf);
        auto cntDescf = CreateContentDescRequest(fbb, SSDescf, tsDescf, vlDescf);
        return cntDescf;
    }

    Status SetContentDesc(const ContentDescRequest *cntDescf, ContentDesc &cntDesc) {
        // Deserialize space Desc
        std::vector<GeoCoordinate> geoCoor;
        for(int j = 0; j < cntDescf->ssdesc()->perimeter_latitude()->size(); j++) {
            GeoCoordinate geo;
            geo.latitude = cntDescf->ssdesc()->perimeter_latitude()->Get(j);
            geo.logitude = cntDescf->ssdesc()->perimeter_logitude()->Get(j);
            geoCoor.push_back(geo);
        }

        cntDesc.setSpaceDesc(cntDescf->ssdesc()->geo_names()->str(),
                        cntDescf->ssdesc()->adcode()->str(),
                        cntDescf->ssdesc()->logitude(),
                        cntDescf->ssdesc()->latitude(),
                        geoCoor);

        // Deserialize time Desc
        cntDesc.setTimeSlotDesc(cntDescf->tsdesc()->report_t(),
                        cntDescf->tsdesc()->start_t(),
                        cntDescf->tsdesc()->end_t(),
                        cntDescf->tsdesc()->interval(),
                        cntDescf->tsdesc()->count());

        // Deserialize var Desc
        std::unordered_map<std::string, std::string> globalAttrs;
        for(int k = 0; k < cntDescf->vldesc()->attrs()->size(); k++) {
            std::string attrName = cntDescf->vldesc()->attrs()->Get(k)->attr_name()->str();
            std::string attrVal = cntDescf->vldesc()->attrs()->Get(k)->attr_val()->str();
            globalAttrs.insert({attrName, attrVal});
        }

        cntDesc.setVarListDesc(cntDescf->vldesc()->group_name()->str(), 
                                cntDescf->vldesc()->group_len(), globalAttrs);

        std::vector<VarDesc> varDesc;
        for(int j = 0; j < cntDescf->vldesc()->vars()->size(); j++) {
            VarDesc desc;
            Dimes shape;
            for(int k = 0; k < cntDescf->vldesc()->vars()->Get(j)->shape()->size(); k++) {
                shape.push_back(cntDescf->vldesc()->vars()->Get(j)->shape()->Get(k));
            }

            std::unordered_map<std::string, std::string> attrs;
            for(int k = 0; k < cntDescf->vldesc()->vars()->Get(j)->attrs()->size(); k++) {
                std::string attrName = cntDescf->vldesc()->vars()->Get(j)->attrs()->Get(k)->attr_name()->str();
                std::string attrVal = cntDescf->vldesc()->vars()->Get(j)->attrs()->Get(k)->attr_val()->str();
                attrs.insert({attrName, attrVal});
            }

            desc.setVarDesc(cntDescf->vldesc()->vars()->Get(j)->var_name()->str(),
                    cntDescf->vldesc()->vars()->Get(j)->var_len(),
                    cntDescf->vldesc()->vars()->Get(j)->res_ration(),
                    cntDescf->vldesc()->vars()->Get(j)->var_type()->str(),
                    shape,
                    cntDescf->vldesc()->vars()->Get(j)->nc_var_id(),
                    cntDescf->vldesc()->vars()->Get(j)->nc_group_id(),
                    cntDescf->vldesc()->vars()->Get(j)->group_path()->str(),
                    attrs);
            varDesc.push_back(desc);   
        }
        cntDesc.setVarListVarDesc(varDesc);
        return Status::OK();
    }

    flatbuffers::Offset<FilePathListRequest> GetFilePathList(flatbuffers::FlatBufferBuilder &fbb, FilePathList *fileList) {
        auto dirPathf = fbb.CreateString(fileList->dirPath);
        auto sitePathf = fbb.CreateString(fileList->sitePath);
        std::vector<flatbuffers::Offset<flatbuffers::String>> pathfVector;
        for(auto path : fileList->fileNames) {
            auto pathf = fbb.CreateString(path);
            pathfVector.push_back(pathf);
        }
        auto pathfVectorf = fbb.CreateVector(pathfVector);
        std::vector<flatbuffers::Offset<StoreSiteRequest>> sitesVector;
        for(auto site: fileList->sites) {
            auto childSitef =  GetSite(fbb, site);
            sitesVector.push_back(childSitef);
        }
        auto sitesVectorf = fbb.CreateVector(sitesVector);
        auto filePathListf = CreateFilePathListRequest(fbb, dirPathf, sitesVectorf, sitePathf, pathfVectorf); 
        return filePathListf;
    }


    Status SetFilePathList(const FilePathListRequest *fileListf, FilePathList *pathList) {
        pathList->dirPath = fileListf->dir_path()->c_str();
        pathList->sitePath =  fileListf->site_path()->c_str();
        auto pathVector =  fileListf->file_name();
        for(int j = 0; j < pathVector->size(); j++) {
            pathList->fileNames.push_back(pathVector->Get(j)->c_str());
        }
        auto siteVector = fileListf->sites();
        SetSite(siteVector, &(pathList->sites));
        return Status::OK();
    }

    flatbuffers::Offset<StoreDescRequest> GetStoreDesc(flatbuffers::FlatBufferBuilder &fbb, StoreDesc &storeDesc) {
        auto SSNamef = fbb.CreateString(storeDesc.SSName);
        auto kindf = fbb.CreateString(storeDesc.getStoreKind());
        auto rootPathf = fbb.CreateString(storeDesc.conConf.rootPath);
        auto storeDescf = CreateStoreDescRequest(fbb, SSNamef, storeDesc.writable, storeDesc.size,
                                                    storeDesc.capacity, kindf, rootPathf);
        return storeDescf;
    }

    Status SetStoreDesc(const StoreDescRequest *storeDescf, StoreDesc &storeDesc) {
        storeDesc.SSName = storeDescf->ssname()->c_str();
        storeDesc.writable = storeDescf->writable();
        storeDesc.size = storeDescf->size();
        storeDesc.capacity = storeDescf->capacity();
        storeDesc.setStoreKind(storeDescf->kind()->c_str());
        storeDesc.conConf.rootPath = storeDescf->root_path()->c_str();
        return Status::OK();
    }
}