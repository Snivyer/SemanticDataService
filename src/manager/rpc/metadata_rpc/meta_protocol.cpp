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
        for(int i = 0; i <cntIDVec.size(); i++) {
            auto spaceIDf = fbb.CreateString(cntIDVec[i].getSpaceID());
            auto timeIDf = fbb.CreateString(cntIDVec[i].getTimeID());
            auto varIDf = fbb.CreateString(cntIDVec[i].getVarID());
            auto cntIDf = CreateContentIDRequest(fbb, spaceIDf, timeIDf, varIDf);
            cntIDfVec.push_back(cntIDf);
        }
        auto cntIDfVecf = fbb.CreateVector(cntIDfVec);

        // Serialize Content Decription
        for(int i = 0; i < cntDescVec.size(); i++) {

            // Serialize Space Description
            auto geoNamef = fbb.CreateString(cntDescVec[i].ssDesc.geoName);
            auto adcodef = fbb.CreateString(cntDescVec[i].ssDesc.adCode);
            std::vector<double> geo_logitude;
            std::vector<double> geo_latitude;

            for(auto item : cntDescVec[i].ssDesc.geoPerimeter) {
                geo_logitude.push_back(item.logitude);
                geo_latitude.push_back(item.latitude);
            }
            auto geo_logitude_vec = fbb.CreateVector(geo_logitude);
            auto geo_latitude_vec = fbb.CreateVector(geo_latitude);

            auto SSDescf = CreateSSDescRequest(fbb, geoNamef, adcodef, cntDescVec[i].ssDesc.geoCentral.logitude,
                                            cntDescVec[i].ssDesc.geoCentral.latitude, geo_logitude_vec, geo_latitude_vec);
            
            // Serialize Time Decription
            time_t reportT = std::mktime(&(cntDescVec[i].tsDesc.reportT));
            time_t startT = std::mktime(&(cntDescVec[i].tsDesc.startT));
            time_t endT = std::mktime(&(cntDescVec[i].tsDesc.endT));
            auto tsDescf = CreateTSDescRequest(fbb, reportT, startT, endT,
                                                cntDescVec[i].tsDesc.interval, cntDescVec[i].tsDesc.count);

            // Serialize Var Description
            auto groupNamef = fbb.CreateString(cntDescVec[i].vlDesc.groupName);
            std::vector<flatbuffers::Offset<VarDescRequest>> varDescfVec;
            for(auto item : cntDescVec[i].vlDesc.desc) {
                auto varNamef = fbb.CreateString(item.varName);
                auto varTypef = fbb.CreateString(item.varType);
                auto shapeVecf = fbb.CreateVector(item.shape);
                auto varDescf = CreateVarDescRequest(fbb, varNamef, varTypef, item.varLen,
                                                        item.resRation, shapeVecf, item.ncVarID, item.ncGroupID);
                varDescfVec.push_back(varDescf);
            }
            auto varDescfVecf = fbb.CreateVector(varDescfVec);
            auto vlDescf = CreateVLDescRequest(fbb, groupNamef, cntDescVec[i].vlDesc.groupLen, varDescfVecf);

            auto cntDescf = CreateContentDescRequest(fbb, SSDescf, tsDescf, vlDescf);
            cntDescfVec.push_back(cntDescf);
        }
        auto cntDescfVecf = fbb.CreateVector(cntDescfVec);
        auto message = CreateSemanticSpaceLoadReply(fbb, spaceInfof, cntIDfVecf, cntDescfVecf);
        return messageSend(sock, MessageTypeSemanticSpaceLoadReply, &fbb, message);
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
            cntID.setSpaceID(cntIDVecf->Get(i)->space_id()->str());
            cntID.setTimeID(cntIDVecf->Get(i)->time_id()->str());
            cntID.setVarID(cntIDVecf->Get(i)->var_id()->str());

            ContentDesc cntDesc;
            // Deserialize space Desc
            std::vector<GeoCoordinate> geoCoor;
            for(int j = 0; j < cntDescVecf->Get(i)->ssdesc()->perimeter_latitude()->size(); j++) {
                GeoCoordinate geo;
                geo.latitude = cntDescVecf->Get(i)->ssdesc()->perimeter_latitude()->Get(j);
                geo.logitude = cntDescVecf->Get(i)->ssdesc()->perimeter_logitude()->Get(j);
                geoCoor.push_back(geo);
            }

            cntDesc.setSpaceDesc(cntDescVecf->Get(i)->ssdesc()->geo_names()->str(),
                                    cntDescVecf->Get(i)->ssdesc()->adcode()->str(),
                                    cntDescVecf->Get(i)->ssdesc()->logitude(),
                                    cntDescVecf->Get(i)->ssdesc()->latitude(),
                                    geoCoor);
            
            // Deserialize time Desc
            cntDesc.setTimeSlotDesc(cntDescVecf->Get(i)->tsdesc()->report_t(),
                                    cntDescVecf->Get(i)->tsdesc()->start_t(),
                                    cntDescVecf->Get(i)->tsdesc()->end_t(),
                                    cntDescVecf->Get(i)->tsdesc()->interval(),
                                    cntDescVecf->Get(i)->tsdesc()->count());

            // Deserialize var Desc
            cntDesc.setVarListDesc(cntDescVecf->Get(i)->vldesc()->group_name()->str(), 
                                    cntDescVecf->Get(i)->vldesc()->group_len());
            
            std::vector<VarDesc> varDesc;
            for(int j = 0; j < cntDescVecf->Get(i)->vldesc()->vars()->size(); j++) {
                VarDesc desc;
                Dimes shape;
                for(int k = 0; k < cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->shape()->size(); k++) {
                    shape.push_back(cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->shape()->Get(k));
                }

                desc.setVarDesc(cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->var_name()->str(),
                                cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->var_len(),
                                cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->res_ration(),
                                cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->var_type()->str(),
                                shape,
                                cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->nc_group_id(),
                                cntDescVecf->Get(i)->vldesc()->vars()->Get(j)->nc_var_id());
               varDesc.push_back(desc);   
            }

            cntDesc.setVarListVarDesc(varDesc);
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
        auto poolNamef = fbb.CreateString(config.poolName);
        auto rootPathf = fbb.CreateString(config.rootPath);
        auto message = CreateStorageSpaceCreateRequest(fbb, spaceNamef, writable, storageKindf,
                                                        capacitySize, spaceIDf, userNamef, confFilef,
                                                        poolNamef, rootPathf);
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
        config.poolName = message->pool_name()->str();
        config.rootPath = message->root_path()->str();
        return Status::OK();
    }

    Status SendCreateStorageSpaceReply(int sock, StorageSpace *space){
        flatbuffers::FlatBufferBuilder fbb;
        auto storageIDf = fbb.CreateString(std::to_string(space->storageID));
        auto ssNamef = fbb.CreateString(space->stoMeta.SSName);
        auto poolNamef = fbb.CreateString(space->stoMeta.sysDesc.conConf.poolName);
        auto rootPathf = fbb.CreateString(space->stoMeta.sysDesc.conConf.rootPath);
        flatbuffers::Offset<flatbuffers::String> kindf;

        switch(space->stoMeta.kind) {
            case StoreSpaceKind::BB:
                kindf = fbb.CreateString("BB");
                break;
            case StoreSpaceKind::Ceph:
                kindf = fbb.CreateString("Ceph");
                break;
            case StoreSpaceKind::Lustre:
                kindf = fbb.CreateString("Lustre");
                break;
            case StoreSpaceKind::Local:
                kindf = fbb.CreateString("Local");
                break;
            default: 
                kindf = fbb.CreateString("None");
                break;
        }
        auto message = CreateStorageSpaceCreateReply(fbb, storageIDf, ssNamef, space->stoMeta.writable,
                                                    space->stoMeta.size, space->stoMeta.capacity, kindf,
                                                    poolNamef, rootPathf);
        return messageSend(sock, MessageTypeStorageSpaceCreateReply, &fbb, message);
    }

    Status ReadCreateStorageSpaceReply(uint8_t* data, StorageSpace& space) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceCreateReply>(data);
        space.storageID = std::atoi(message->storage_id()->str().c_str());
        space.stoMeta.SSName = message->ssname()->str();
        space.stoMeta.writable = message->writable();
        space.stoMeta.size = message->size();
        space.stoMeta.capacity = message->capacity();
        space.stoMeta.sysDesc.conConf.rootPath = message->root_path()->str();
        space.stoMeta.sysDesc.conConf.poolName = message->pool_name()->str();

        if(message->kind()->str() == "BB") {
            space.stoMeta.kind = StoreSpaceKind::BB;
        } else if(message->kind()->str() == "Ceph") {
            space.stoMeta.kind = StoreSpaceKind::Ceph;
        } else if(message->kind()->str() == "Lustre") {
            space.stoMeta.kind = StoreSpaceKind::Lustre;
        } else if(message->kind()->str() == "Local") {
            space.stoMeta.kind = StoreSpaceKind::Local;
        } else {
            space.stoMeta.kind = StoreSpaceKind::None;
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

    Status SendSearchContentIndexRequest(int sock, std::vector<std::string> &geoNames, std::vector<std::string> &times, 
                                            std::vector<std::string> &varNames, std::string &groupName) {
        ARROW_LOG(INFO) <<  "Send content index search request:";
        flatbuffers::FlatBufferBuilder fbb;
        std::vector<flatbuffers::Offset<flatbuffers::String>> geoNamesf;
        std::vector<flatbuffers::Offset<flatbuffers::String>> timesf;
        std::vector<flatbuffers::Offset<flatbuffers::String>> varNamesf;

        for(auto name: geoNames) {
            auto namef = fbb.CreateString(name);
            geoNamesf.push_back(namef);
        }

        for(auto time: times) {
            auto timef = fbb.CreateString(time);
            timesf.push_back(timef);
        }

        for(auto name: varNames) {
            auto namef = fbb.CreateString(name);
            varNamesf.push_back(namef);
        }

        auto geoNameVector = fbb.CreateVector(geoNamesf);
        auto timeVector = fbb.CreateVector(timesf);
        auto varNameVector = fbb.CreateVector(varNamesf);
        auto groupNamesf = fbb.CreateString(groupName);

        auto message = CreateContentIndexSearchRequest(fbb, geoNameVector, timeVector, varNameVector, groupNamesf);
        return messageSend(sock, MessageTypeDataSearchRequest, &fbb, message);
    }
    
    Status ReadSearchContentIndexRequest(uint8_t* data, std::vector<std::string> &geoNames, std::vector<std::string> &times,
                                             std::vector<std::string> &varNames, std::string &groupName) {

        DCHECK(data);
        auto message = flatbuffers::GetRoot<ContentIndexSearchRequest>(data);

        auto geoNameVector = message->geo_names();
        auto timeVector = message->times();
        auto varVector = message->var_names();
        groupName = message->group_name()->c_str();

        for(int i = 0; i < geoNameVector->size(); i++) {
            geoNames.push_back(geoNameVector->Get(i)->str());
        }

        for(int i = 0; i < timeVector->size(); i++) {
            times.push_back(timeVector->Get(i)->str());
        }

        for(int i = 0; i < varVector->size(); i++) {
            varNames.push_back(varVector->Get(i)->str());
        }

        return Status::OK();

    }
    Status SendSearchContentIndexReply(int sock, std::string spaceID, std::string timeID, std::string varID) {
        
        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto message = CreateContentIndexSearchReply(fbb, spaceIDf, timeIDf, varIDf);
        return messageSend(sock, MessageTypeDataSearchReply, &fbb, message);        

    }

    Status ReadSearchContentIndexReply(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<ContentIndexSearchReply>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
        return Status::OK(); 
    }

}