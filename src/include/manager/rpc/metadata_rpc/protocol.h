#pragma once

#include <vector>

#include "flatbuffers/flatbuffers.h"
#include "arrow/status.h"
#include "arrow/util/logging.h"
#include "abstract/IO/io.h"
#include "abstract/meta/cnt_ID.h"
#include "manager/proto/meta_generated.h"
#include "manager/databox/databox_object.h"




using arrow::Status;


namespace SDS {

    const int64_t MessageTypeConnectRequest     = 1;
    const int64_t MessageTypeConnectReply       = 2;
    const int64_t MessageTypeCreateRequest      = 3;
    const int64_t MessageTypeCreateReply        = 4;
    const int64_t MessageTypeGetRequest         = 5;
    const int64_t MessageTypeGetReply           = 6;
    const int64_t MessageTypeReleaseRequest     = 7;
    const int64_t MessageTypeReleaseReply       = 8;
    const int64_t MessageTypeContaineRequest    = 9;
    const int64_t MessageTypeContaineReply      = 10;
    const int64_t MessageTypeDeleteRequest      = 11;
    const int64_t MessageTypeDeleteReply        = 12;
    const int64_t MessageTypeDisconnectRequest  = 13;
    const int64_t MessageTypeDisconnectReply    = 14;


    Status messageReceive(int sock, int64_t message_type, std::vector<uint8_t>* buffer);
    

    /* connect message functions*/

    Status SendConnectRequest(int sock);
    Status ReadConnectRequest(uint8_t* data);
    Status SendConnectReply(int sock, int64_t memory_capacity);
    Status ReadConnectReply(uint8_t* data, int64_t* memory_capacity);

    /* databox message functions*/
    Status SendCreateRequest(int sock, std::string spaceID, std::string timeID, std::string varID, std::string path);
    Status ReadCreateRequest(uint8_t* data,std::string &spaceID, std::string &timeID, std::string &varID, std::string &path);
    Status SendCreateReply(int sock, DBMeta &dbMeta);
    Status ReadCreateReply(uint8_t* data, DBMeta &dbMeta);

    Status SendGetRequest(int sock, std::string spaceID, std::string timeID, std::string varID, int64_t timeout);
    Status ReadGetRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID, int64_t &timeout);
    Status SendGetReply(int sock, std::string ip, int port);
    Status ReadGetReply(uint8_t* data, std::string &ip, int &port);

    Status SendContainRequest(int sock, std::string spaceID, std::string timeID, std::string varID);
    Status ReadContainRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID);
    Status SendContainReply(int sock, bool is_contain);
    Status ReadContainReply(uint8_t* data, bool &is_contain);

    Status SendReleaseRequest(int sock, std::string spaceID, std::string timeID, std::string varID);
    Status ReadReleaseRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID);
    Status SendReleaseReply(int sock, bool is_release);
    Status ReadReleaseReply(uint8_t* data, bool &is_release);

    Status SendDeleteRequest(int sock, std::string spaceID, std::string timeID, std::string varID);
    Status ReadDeleteRequest(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID);
    Status SendDeleteReply(int sock, bool is_release);
    Status ReadDeleteReply(uint8_t* data, bool &is_release);

};
