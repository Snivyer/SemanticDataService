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

    Status SendCreateRequest(int sock, std::string spaceID, std::string timeID, 
                            std::string varID, std::string path) {
        ARROW_LOG(INFO) <<  "Send create request, cntID:" << spaceID <<" " << timeID <<" " << varID;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto pathf = fbb.CreateString(path);
        auto message = CreateDBCreateRequest(fbb, spaceIDf, timeIDf, varIDf, pathf);
        return messageSend(sock, MessageTypeCreateRequest, &fbb, message);
    }

    Status ReadCreateRequest(uint8_t* data, std::string &spaceID, std::string &timeID, 
                            std::string &varID, std::string &path) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBCreateRequest>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
        path = message->path()->str();
        return Status::OK();
    }
 
    Status SendCreateReply(int sock, DBMeta &dbMeta) {
        flatbuffers::FlatBufferBuilder fbb;
        std::vector<std::string> varNames;
        std::vector<flatbuffers::Offset<flatbuffers::String>> names;

        for(auto item: dbMeta.varList) {
            auto name = fbb.CreateString(item.varName);
            names.push_back(name);
        }

        auto nameVector = fbb.CreateVector(names);
        auto message = CreateDBCreateReply(fbb, dbMeta.stepCount, dbMeta.varCount,
                                            dbMeta.varLen, nameVector);
        return messageSend(sock, MessageTypeCreateReply, &fbb, message);
    }


    Status ReadCreateReply(uint8_t* data, DBMeta &dbMeta) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBCreateReply>(data);
        dbMeta.stepCount = message->step_count();
        dbMeta.varCount = message->var_count();
        dbMeta.varLen = message->var_len();
        auto nameList = message->var_name();

        for(int i = 0; i < nameList->size(); i++) {
            VarDesc varDesc;
            varDesc.varName = nameList->Get(i)->str();
            dbMeta.varList.push_back(varDesc);
        }

        return Status::OK();   
    }

    Status SendGetRequest(int sock, std::string spaceID, std::string timeID, std::string varID, int64_t timeout) {
        flatbuffers::FlatBufferBuilder fbb;
        ARROW_LOG(INFO) <<  "Send get request, cntID:" << spaceID <<" " << timeID <<" " << varID;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto message = CreateDBgetRequest(fbb, spaceIDf, timeIDf, varIDf, timeout);
        return messageSend(sock, MessageTypeGetRequest, &fbb, message);
    }

    Status ReadGetRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID, int64_t &timeout) {
        
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBgetRequest>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
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

    Status SendContainRequest(int sock, std::string spaceID, std::string timeID, 
                            std::string varID) {
        ARROW_LOG(INFO) <<  "Send contain request, cntID:" << spaceID <<" " << timeID <<" " << varID;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto message = CreateDBcontainRequest(fbb, spaceIDf, timeIDf, varIDf);
        return messageSend(sock, MessageTypeConnectRequest, &fbb, message);
    }

    Status ReadContainRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBcontainRequest>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
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

    Status SendReleaseRequest(int sock, std::string spaceID, std::string timeID, 
                            std::string varID) {
        ARROW_LOG(INFO) <<  "Send release request, cntID:" << spaceID <<" " << timeID <<" " << varID;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto message = CreateDBReleaseRequest(fbb, spaceIDf, timeIDf, varIDf);
        return messageSend(sock, MessageTypeReleaseRequest, &fbb, message);
    }

    Status ReadReleaseRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBReleaseRequest>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
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

    Status SendDeleteRequest(int sock, std::string spaceID, std::string timeID, 
                            std::string varID) {
        ARROW_LOG(INFO) <<  "Send release request, cntID:" << spaceID <<" " << timeID <<" " << varID;

        flatbuffers::FlatBufferBuilder fbb;
        auto spaceIDf = fbb.CreateString(spaceID);
        auto timeIDf = fbb.CreateString(timeID);
        auto varIDf = fbb.CreateString(varID);
        auto message = CreateDBDeleteRequest(fbb, spaceIDf, timeIDf, varIDf);
        return messageSend(sock, MessageTypeDeleteRequest, &fbb, message);
    }

    Status ReadDeleteRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID) {
        DCHECK(data);
        auto message = flatbuffers::GetRoot<DBDeleteRequest>(data);
        spaceID = message->space_id()->str();
        timeID = message->time_id()->str();
        varID = message->var_id()->str();
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






    


    
}