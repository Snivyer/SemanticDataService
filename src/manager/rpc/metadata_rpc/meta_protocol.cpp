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
 
    Status SendCreateSemanticSpaceReply(int sock, std::string spaceID) {
        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto message = CreateSemanticSpaceCreateReply(fbb, spaceIDf);
        return messageSend(sock, MessageTypeSemanticSpaceCreateReply, &fbb, message);
    }

    Status ReadCreateSemanticSpaceReply(uint8_t* data, std::string &spaceID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<SemanticSpaceCreateReply>(data);
        spaceID = message->space_id()->str();
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
        auto message = CreateStorageSpaceCreateRequest(fbb, spaceNamef, writable, storageKindf, capacitySize, spaceIDf, userNamef, confFilef, poolNamef, rootPathf);
        
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

    Status SendCreateStorageSpaceReply(int sock, std::string storageID) {
        flatbuffers::FlatBufferBuilder fbb;
        auto storageIDf = fbb.CreateString(storageID);
        auto message = CreateStorageSpaceCreateReply(fbb, storageIDf);
        return messageSend(sock, MessageTypeStorageSpaceCreateReply, &fbb, message);
    }

    Status ReadCreateStorageSpaceReply(uint8_t* data, std::string &storageID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<StorageSpaceCreateReply>(data);
        storageID = message->storage_id()->str();
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

    Status SendCreateContentIndexReply(int sock, int64_t welcome) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateContentIndexReply(fbb, welcome);
        return messageSend(sock, MessageTypeDataImportFromLocalReply, &fbb, message);
    }

    Status ReadCreateContentIndexReply(uint8_t* data, int64_t* welcome) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<ContentIndexReply>(data);
        *welcome = message->welcome();
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