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

    const int64_t MessageTypeConnectRequest = 1;
    const int64_t MessageTypeConnectReply   = 2;
    const int64_t MessageTypeCreateRequest  = 3;
    const int64_t MessageTypeCreateReply    = 4;
    const int64_t MessageTypeGetRequest     = 5;
    const int64_t MessageTypeGetReply       = 6;



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

};
