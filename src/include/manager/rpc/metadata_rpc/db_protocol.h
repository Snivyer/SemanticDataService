/* Author: Snivyer
*  Create on: 2024/11/2
*  Description:
   protocol to send or receive socket message about operations of databox
*/
#pragma once
#include <vector>
#include "flatbuffers/flatbuffers.h"
#include "arrow/status.h"
#include "arrow/util/logging.h"
#include "abstract/IO/io.h"
#include "abstract/meta/cnt_ID.h"
#include "manager/proto/db_flb_generated.h"
#include "manager/databox/databox_object.h"
#include "manager/rpc/metadata_rpc/meta_protocol.h"


using arrow::Status;


namespace SDS {

    const int64_t MessageTypeConnectRequest      = 1;
    const int64_t MessageTypeConnectReply        = 2;
    const int64_t MessageTypeCreateRequest       = 3;
    const int64_t MessageTypeCreateReply         = 4;
    const int64_t MessageTypeGetRequest          = 5;
    const int64_t MessageTypeGetReply            = 6;
    const int64_t MessageTypeReleaseRequest      = 7;
    const int64_t MessageTypeReleaseReply        = 8;
    const int64_t MessageTypeContaineRequest     = 9;
    const int64_t MessageTypeContaineReply       = 10;
    const int64_t MessageTypeDeleteRequest       = 11;
    const int64_t MessageTypeDeleteReply         = 12;
    const int64_t MessageTypeDisconnectRequest   = 13;
    const int64_t MessageTypeDisconnectReply     = 14;
    const int64_t MessageTypeGetContentIDRequest = 15;
    const int64_t MessageTypeGetContentIDReply   = 16;

    Status messageReceive(int sock, int64_t message_type, std::vector<uint8_t>* buffer);

    /* connect message functions*/
    Status SendConnectRequest(int sock);
    Status ReadConnectRequest(uint8_t* data);
    Status SendConnectReply(int sock, int64_t memory_capacity);
    Status ReadConnectReply(uint8_t* data, int64_t* memory_capacity);

    /* databox message functions*/
    Status SendCreateRequest(int sock, ContentID &cntID, ContentDesc &cntDesc, StoreDesc &stoDesc, FilePathList &fileList);
    Status ReadCreateRequest(uint8_t* data, ContentID &cntID, ContentDesc &cntDesc, StoreDesc &stoDesc, FilePathList &fileList);
    Status SendCreateReply(int sock, DBMeta &dbMeta);
    Status ReadCreateReply(uint8_t* data, DBMeta &dbMeta);

    Status SendGetRequest(int sock, std::vector<size_t> &ids, int64_t timeout);
    Status ReadGetRequest(uint8_t* data, std::vector<size_t> &ids, int64_t &timeout);
    Status SendGetReply(int sock, std::string ip, int port);
    Status ReadGetReply(uint8_t* data, std::string &ip, int &port);

    Status SendContainRequest(int sock, size_t id);
    Status ReadContainRequest(uint8_t* data, size_t &id);
    Status SendContainReply(int sock, bool is_contain);
    Status ReadContainReply(uint8_t* data, bool &is_contain);

    Status SendReleaseRequest(int sock, size_t id);
    Status ReadReleaseRequest(uint8_t* data, size_t &id);
    Status SendReleaseReply(int sock, bool is_release);
    Status ReadReleaseReply(uint8_t* data, bool &is_release);

    Status SendDeleteRequest(int sock, std::vector<size_t> &ids);
    Status ReadDeleteRequest(uint8_t* data, std::vector<size_t> &ids);
    Status SendDeleteReply(int sock, bool is_release);
    Status ReadDeleteReply(uint8_t* data, bool &is_release);

    Status SendGetContentIDRequest(int sock, size_t id);
    Status ReadGetContentIDRequest(uint8_t* data, size_t &id);
    Status SendGetContentIDReply(int sock, ContentID &cntID);
    Status ReadGetContentIDReply(uint8_t* data, ContentID &cntID);

    flatbuffers::Offset<DataBoxMetaRequest> GetDBMeta(flatbuffers::FlatBufferBuilder &fbb, DBMeta &dbMeta);
    Status SetDBMeta(const DataBoxMetaRequest *dbMetaf, DBMeta &dbMeta);
        


};
