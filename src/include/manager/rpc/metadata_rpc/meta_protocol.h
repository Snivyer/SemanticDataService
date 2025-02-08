/* Author: Snivyer
*  Create on: 2024/11/19
*  Description:
   protocol to send or receive socket message about operations of metadata services
*/

#pragma once
#include <vector>

#include "flatbuffers/flatbuffers.h"
#include "arrow/status.h"
#include "arrow/util/logging.h"
#include "abstract/IO/io.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/meta/sto_meta_template.h"
#include "manager/proto/meta_flb_generated.h"
#include "manager/rpc/metadata_rpc/db_protocol.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"


using arrow::Status;

namespace SDS {

    const int64_t MessageTypeMetaConnectRequest              = 1;
    const int64_t MessageTypeMetaConnectReply                = 2;
    const int64_t MessageTypeMetaDisconnectRequest           = 3;
    const int64_t MessageTypeMetaDisconnectReply             = 4;
    const int64_t MessageTypeSemanticSpaceCreateRequest      = 5;
    const int64_t MessageTypeSemanticSpaceCreateReply        = 6;
    const int64_t MessageTypeStorageSpaceCreateRequest       = 7;
    const int64_t MessageTypeStorageSpaceCreateReply         = 8;
    const int64_t MessageTypeDataImportFromLocalRequest      = 9;
    const int64_t MessageTypeDataImportFromLocalReply        = 10;
    const int64_t MessageTypeDataImportFromOtherRequest      = 11;
    const int64_t MessageTypeDataImportFromOtherReply        = 12;
    const int64_t MessageTypeDataSearchRequest               = 13;
    const int64_t MessageTypeDataSearchReply                 = 14;
    const int64_t MessageTypeSemanticSpaceLoadRequest      = 15;
    const int64_t MessageTypeSemanticSpaceLoadReply        = 16;



   

    /* connect message functions*/
    Status SendMetaConnectRequest(int sock);
    Status ReadMetaConnectRequest(uint8_t* data);
    Status SendMetaConnectReply(int sock, int64_t welcome);
    Status ReadMetaConnectReply(uint8_t* data, int64_t* welcome);

    /* create semantic space message functions*/
    Status SendCreateSemanticSpaceRequest(int sock, std::string spaceName, std::vector<std::string> geoNames);
    Status ReadCreateSemanticSpaceRequest(uint8_t* data, std::string &spaceName, std::vector<std::string> &geoNames);
    Status SendCreateSemanticSpaceReply(int sock, SemanticSpace* space);
    Status ReadCreateSemanticSpaceReply(uint8_t* data, SemanticSpace &space);

    /*load semantic space message functions*/
    Status SendLoadSemanticSpaceRequest(int sock, std::string spaceName);
    Status ReadLoadSemanticSpaceRequest(uint8_t* data, std::string &spaceName);
    Status SendLoadSemanticSpaceReply(int sock, SemanticSpace* space);
    Status ReadLoadSemanticSpaceReply(uint8_t* data, SemanticSpace &space);

    /* create storage space message functions*/
    Status SendCreateStorageSpaceRequest(int sock, std::string SSName, size_t capacitySize, std::string spaceID, std::string storageKind,
                                         bool writable, ConnectConfig &config);
    Status ReadCreateStorageSpaceRequest(uint8_t* data, std::string& SSName, size_t &capacitySize, std::string& spaceID, std::string &storageKind,
                                         bool &writable, ConnectConfig &config);
    Status SendCreateStorageSpaceReply(int sock, StorageSpace* space);
    Status ReadCreateStorageSpaceReply(uint8_t* data, StorageSpace &space);

    /* create content index message functions*/
    Status SendCreateContentIndexRequest(int sock, std::string semanticSpaceName, std::string storageSpaceName, std::string dirPath);
    Status ReadCreateContentIndexRequest(uint8_t* data, std::string &semanticSpaceName, std::string &storageSpaceName, std::string &dirPath);
    Status SendCreateContentIndexReply(int sock, int64_t welcome);
    Status ReadCreateContentIndexReply(uint8_t* data, int64_t* welcome);

    /* search content index message functions*/
    Status SendSearchContentIndexRequest(int sock, std::vector<std::string> &geoNames, std::vector<std::string> &times, std::vector<std::string> &varNames, std::string &groupName);
    Status ReadSearchContentIndexRequest(uint8_t* data, std::vector<std::string> &geoNames, std::vector<std::string> &times, std::vector<std::string> &varNames, std::string &groupName);
    Status SendSearchContentIndexReply(int sock, std::string spaceID, std::string timeID, std::string varID);
    Status ReadSearchContentIndexReply(uint8_t* data, std::string &spaceID, std::string &timeID, std::string &varID);


};
