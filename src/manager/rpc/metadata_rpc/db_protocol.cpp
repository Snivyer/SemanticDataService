#include "manager/rpc/metadata_rpc/db_protocol.h"


namespace SDS {


    Status messageReceive(int sock, int64_t message_type, std::vector<uint8_t>* buffer) {
        int64_t type;
        RETURN_NOT_OK(ReadMessage(sock, &type, buffer));
        ARROW_CHECK(type == message_type) << "type = " << type
                                    << ", message_type = " << message_type;
         return Status::OK();
    }

    template <typename Message>
    Status messageSend(int sock, int64_t message_type, flatbuffers::FlatBufferBuilder* fbb,
                        const Message& message) {
        fbb->Finish(message);
        return WriteMessage(sock, message_type, fbb->GetSize(), fbb->GetBufferPointer());
    }


    Status SendConnectRequest(int sock) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateClientConnectRequest(fbb, sock);
        return messageSend(sock, MessageTypeConnectRequest, &fbb, message);
    }


    Status ReadConnectRequest(uint8_t* data) {
        return Status::OK();

    }

    Status SendConnectReply(int sock, int64_t memoryCapacity) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateClientConnectReply(fbb, memoryCapacity);
        return messageSend(sock, MessageTypeConnectReply, &fbb, message);
    }


    Status ReadConnectReply(uint8_t* data, int64_t* memoryCapacity) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<ClientConnectReply>(data);
        *memoryCapacity = message->capacity();
        return Status::OK();
    }

    Status SendCreateRequest(int sock, ContentID &cntID, ContentDesc &cntDesc,
                                StoreDesc &stoDesc, FilePathList &fileList) {
        ARROW_LOG(INFO) <<  "Send databox create request, cntID:" << cntID.getSpaceID()
                         <<" " << cntID.getTimeID() << " " << cntID.getVarID();

        flatbuffers::FlatBufferBuilder fbb;
        auto cntIDf = GetContentID(fbb, cntID);
        auto cntDescf = GetContentDesc(fbb, cntDesc);
        auto stoDescf = GetStoreDesc(fbb, stoDesc);
        auto fileListf = GetFilePathList(fbb, &fileList);
        auto message = CreateDBCreateRequest(fbb, cntIDf, cntDescf, stoDescf, fileListf);
        return messageSend(sock, MessageTypeCreateRequest, &fbb, message);
    }

    Status ReadCreateRequest(uint8_t* data, ContentID &cntID, ContentDesc &cntDesc,
                                StoreDesc &storeDesc, FilePathList &fileList) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBCreateRequest>(data);
        SetContentID(message->cnt_id(), cntID);
        SetContentDesc(message->cnt_desc(), cntDesc);
        SetStoreDesc(message->sto_desc(), storeDesc);
        SetFilePathList(message->file_path_list(), &fileList);
        return Status::OK();
    }
 
    Status SendCreateReply(int sock, DBMeta &dbMeta) {
        flatbuffers::FlatBufferBuilder fbb;
        auto dbMetaf = GetDBMeta(fbb, dbMeta);
        auto message = CreateDBCreateReply(fbb, dbMetaf);
        return messageSend(sock, MessageTypeCreateReply, &fbb, message);
    }


    Status ReadCreateReply(uint8_t* data, DBMeta &dbMeta) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBCreateReply>(data);
        SetDBMeta(message->db_meta(), dbMeta);
        return Status::OK();   
    }

    Status SendGetRequest(int sock, std::vector<size_t> &ids, int64_t timeout) {
        flatbuffers::FlatBufferBuilder fbb;
        ARROW_LOG(INFO) <<  "Send get request";
        auto idsf = fbb.CreateVector(ids);
        auto message = CreateDBgetRequest(fbb, idsf, timeout);
        return messageSend(sock, MessageTypeGetRequest, &fbb, message);
    }

    Status ReadGetRequest(uint8_t* data, std::vector<size_t> &ids, int64_t &timeout) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBgetRequest>(data);
        for(int i = 0; i < message->ids()->size(); i++) {
            ids.push_back(message->ids()->Get(i));
        }
        timeout = message->timeout();
        return Status::OK(); 
    }

    Status SendGetReply(int sock, std::string ip, int port) {
        flatbuffers::FlatBufferBuilder fbb;
        auto ipf = fbb.CreateString(ip);
        auto message = CreateDBgetReply(fbb, ipf, port);
        return messageSend(sock, MessageTypeGetReply, &fbb, message);
    }

    Status ReadGetReply(uint8_t* data, std::string &ip, int &port) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBgetReply>(data);
        ip = message->ip()->str();
        port = message->port();
        return Status::OK();
    }

    Status SendContainRequest(int sock, size_t id) {
        ARROW_LOG(INFO) <<  "Send contain request";
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateDBcontainRequest(fbb, id);
        return messageSend(sock, MessageTypeConnectRequest, &fbb, message);
    }

    Status ReadContainRequest(uint8_t* data, size_t &id) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBcontainRequest>(data);
        id = message->id();
        return Status::OK(); 
    }

    Status SendContainReply(int sock, bool is_contain) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateDBcontainReply(fbb, is_contain);
        return messageSend(sock, MessageTypeContaineReply, &fbb, message);
    }

    Status ReadContainReply(uint8_t* data, bool &is_contain) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBcontainReply>(data);
        is_contain = message->is_contain();
        return Status::OK();
    }

    Status SendReleaseRequest(int sock, size_t id) {
        ARROW_LOG(INFO) <<  "Send release request";
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateDBReleaseRequest(fbb, id);
        return messageSend(sock, MessageTypeReleaseRequest, &fbb, message);
    }

    Status ReadReleaseRequest(uint8_t* data, size_t &id) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBReleaseRequest>(data);
        id = message->id();
        return Status::OK(); 
    }

    Status SendReleaseReply(int sock, bool is_release) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateDBReleaseReply(fbb, is_release);
        return messageSend(sock, MessageTypeReleaseReply, &fbb, message);
    }

    Status ReadReleaseReply(uint8_t* data, bool &is_release) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBReleaseReply>(data);
        is_release = message->is_release();
        return Status::OK();
    }

    Status SendDeleteRequest(int sock, std::vector<size_t> &ids) {
        ARROW_LOG(INFO) <<  "Send release request";                   
        flatbuffers::FlatBufferBuilder fbb;
        auto idsf = fbb.CreateVector(ids);
        auto message = CreateDBDeleteRequest(fbb, idsf);
        return messageSend(sock, MessageTypeDeleteRequest, &fbb, message);
    }

    Status ReadDeleteRequest(uint8_t* data, std::vector<size_t> &ids) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBDeleteRequest>(data);
        for(int i = 0; i < message->ids()->size(); i++) {
            ids.push_back(message->ids()->Get(i));
        }
        return Status::OK(); 
    }

    Status SendDeleteReply(int sock, bool is_delete) {
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateDBDeleteReply(fbb, is_delete);
        return messageSend(sock, MessageTypeDeleteReply, &fbb, message);
    }

    Status ReadDeleteReply(uint8_t* data, bool &is_delete) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBDeleteReply>(data);
        is_delete = message->is_delete();
        return Status::OK();
    }

    Status SendGetContentIDRequest(int sock, size_t dbID) {
        ARROW_LOG(INFO) <<  "Send get contentID  request, dbID:" << dbID ;                    
        flatbuffers::FlatBufferBuilder fbb;
        auto message = CreateGetContentIDRequest(fbb, dbID);
        return messageSend(sock, MessageTypeGetContentIDRequest, &fbb, message);
    }

    Status ReadGetContentIDRequest(uint8_t* data, size_t &dbID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<GetContentIDRequest>(data);
        dbID = message->db_id();
        return Status::OK();
    }

    Status SendGetContentIDReply(int sock, ContentID &cntID) {
        flatbuffers::FlatBufferBuilder fbb;
        auto cntIDf = GetContentID(fbb, cntID);
        auto message = CreateGetContentIDReply(fbb, cntIDf);
        return messageSend(sock, MessageTypeGetContentIDReply, &fbb, message);
    }

    Status ReadGetContentIDReply(uint8_t* data, ContentID &cntID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<GetContentIDReply>(data);
        SetContentID(message->cnt_id(), cntID);
        return Status::OK();
    }

    flatbuffers::Offset<DataBoxMetaRequest> GetDBMeta(flatbuffers::FlatBufferBuilder &fbb, DBMeta &dbMeta) {
        auto groupNamef = fbb.CreateString(dbMeta.vlDesc.groupName);
        std::vector<flatbuffers::Offset<AttrRequest>> globalAttrsfVec;
        for(auto item : dbMeta.vlDesc.attrs) {
            auto attrNamef = fbb.CreateString(item.first);
            auto attrValf = fbb.CreateString(item.second);
            auto attrf = CreateAttrRequest(fbb, attrNamef, attrValf);
            globalAttrsfVec.push_back(attrf);
        }
        auto globalAttrsfVecf = fbb.CreateVector(globalAttrsfVec);

        std::vector<flatbuffers::Offset<VarDescRequest>> varDescfVec;
        for(auto item : dbMeta.vlDesc.desc) {
            auto varNamef = fbb.CreateString(item.varName);
            auto varTypef = fbb.CreateString(item.varType);
            auto shapeVecf = fbb.CreateVector(item.shape);
            auto groupPath = fbb.CreateString(item.groupPath);
            std::vector<flatbuffers::Offset<AttrRequest>> attrsfVec;
            for(auto item : dbMeta.vlDesc.attrs) {
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
        auto vlDescf = CreateVLDescRequest(fbb, groupNamef, dbMeta.vlDesc.groupLen, varDescfVecf, globalAttrsfVecf);
        auto message = CreateDataBoxMetaRequest(fbb, dbMeta.id, dbMeta.stepCount, dbMeta.varCount,
                                                dbMeta.varLen, vlDescf, dbMeta.filled);
        return message;

    }

    Status SetDBMeta(const DataBoxMetaRequest *dbMetaf, DBMeta &dbMeta) {

        dbMeta.id = dbMetaf->id();
        dbMeta.filled = dbMetaf->filled();
        dbMeta.stepCount = dbMetaf->step_count();
        dbMeta.varCount = dbMetaf->var_count();
        dbMeta.varLen = dbMetaf->var_len();
        
        std::unordered_map<std::string, std::string> globalAttrs;
        for(int k = 0; k < dbMetaf->vldesc()->attrs()->size(); k++) {
            std::string attrName = dbMetaf->vldesc()->attrs()->Get(k)->attr_name()->str();
            std::string attrVal = dbMetaf->vldesc()->attrs()->Get(k)->attr_val()->str();
            globalAttrs.insert({attrName, attrVal});
        }

        dbMeta.vlDesc.setVLDesc(dbMetaf->vldesc()->group_name()->str(), 
                                    dbMetaf->vldesc()->group_len(), globalAttrs);
        std::vector<VarDesc> varDesc;
        for(int j = 0; j < dbMetaf->vldesc()->vars()->size(); j++) {
            VarDesc desc;
            Dimes shape;
            for(int k = 0; k < dbMetaf->vldesc()->vars()->Get(j)->shape()->size(); k++) {
                shape.push_back(dbMetaf->vldesc()->vars()->Get(j)->shape()->Get(k));
            }

            std::unordered_map<std::string, std::string> attrs;
            for(int k = 0; k < dbMetaf->vldesc()->vars()->Get(j)->attrs()->size(); k++) {
                std::string attrName = dbMetaf->vldesc()->vars()->Get(j)->attrs()->Get(k)->attr_name()->str();
                std::string attrVal = dbMetaf->vldesc()->vars()->Get(j)->attrs()->Get(k)->attr_val()->str();
                attrs.insert({attrName, attrVal});
            }

            desc.setVarDesc(dbMetaf->vldesc()->vars()->Get(j)->var_name()->str(),
                    dbMetaf->vldesc()->vars()->Get(j)->var_len(),
                    dbMetaf->vldesc()->vars()->Get(j)->res_ration(),
                    dbMetaf->vldesc()->vars()->Get(j)->var_type()->str(),
                    shape,
                    dbMetaf->vldesc()->vars()->Get(j)->nc_group_id(),
                    dbMetaf->vldesc()->vars()->Get(j)->nc_var_id(),
                    dbMetaf->vldesc()->vars()->Get(j)->group_path()->str(),
                    attrs);
            varDesc.push_back(desc);   
        }
        dbMeta.vlDesc.setVarListVarDesc(varDesc);
        return Status::OK();
    }

    
}