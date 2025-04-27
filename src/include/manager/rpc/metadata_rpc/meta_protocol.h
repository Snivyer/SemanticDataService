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
    const int64_t MessageTypeErrorReply                      = 0;
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
    const int64_t MessageTypeDataBoxSearchRequest            = 13;
    const int64_t MessageTypeDataBoxSearchReply              = 14;
    const int64_t MessageTypeSemanticSpaceLoadRequest        = 15;
    const int64_t MessageTypeSemanticSpaceLoadReply          = 16;
    const int64_t MessageTypeDataFileSearchRequest           = 17;
    const int64_t MessageTypeDataFileSearchReply             = 18;
    const int64_t MessageTypeStorageSpaceLoadRequest         = 19;
    const int64_t MessageTypeStorageSpaceLoadReply           = 20;
    const int64_t MessageTypeTimeIndexRequest                = 21;
    const int64_t MessageTypeTimeIndexReply                  = 22;
    const int64_t MessageTypeVarIndexRequest                 = 23;
    const int64_t MessageTypeVarIndexReply                   = 24;
    const int64_t MessageTypeBindDataSourceRequest           = 25;
    const int64_t MessageTypeBindDataSourceReply             = 26;

    /* connect message functions*/
    Status SendMetaConnectRequest(int sock);
    Status ReadMetaConnectRequest(uint8_t* data);
    Status SendMetaConnectReply(int sock, int64_t welcome);
    Status ReadMetaConnectReply(uint8_t* data, int64_t* welcome);
    Status ReadErrorReply(uint8_t* data, bool &status);

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


    /*load semantic space message functions*/
    Status SendLoadStorageSpaceRequest(int sock, std::string spaceName);
    Status ReadLoadStorageSpaceRequest(uint8_t* data, std::string &spaceName);
    Status SendLoadStorageSpaceReply(int sock, StorageSpace* space);
    Status ReadLoadStorageSpaceReply(uint8_t* data, StorageSpace &space);

    /* create content index message functions*/
    Status SendCreateContentIndexRequest(int sock, std::string semanticSpaceName, std::string storageSpaceName, std::string dirPath);
    Status ReadCreateContentIndexRequest(uint8_t* data, std::string &semanticSpaceName, std::string &storageSpaceName, std::string &dirPath);
    Status SendCreateContentIndexReply(int sock, bool result);
    Status ReadCreateContentIndexReply(uint8_t* data, bool &result);
     
    /* search content index message functions*/
    Status SendSearchDataBoxRequest(int sock, std::string &SSName, std::vector<std::string> &times, std::vector<std::string> &varName);
    Status ReadSearchDataBoxRequest(uint8_t* data, std::string &SSName, std::vector<std::string> &times, std::vector<std::string> &varNames);
    Status SendSearchDataBoxReply(int sock, std::vector<size_t> &databoxID, std::vector<FilePathList> &filePath);
    Status ReadSearchDataBoxReply(uint8_t* data, std::vector<size_t> &databoxID, std::vector<FilePathList> &filePath);

    /* search data file message functions*/
    Status SendSearchDataFileRequest(int sock, std::string &SSName, std::vector<std::string> &times, std::vector<std::string> &varNames);
    Status ReadSearchDataFileRequest(uint8_t* data, std::string &SSName, std::vector<std::string> &times, std::vector<std::string> &varNames);
    Status SendSearchDataFileReply(int sock, std::vector<FilePathList> &filePath);
    Status ReadSearchDataFileReply(uint8_t* data, std::vector<FilePathList> &filePath);

    /*get index message functions*/
    Status SendTimeIndexRequest(int sock);
    Status SendTimeIndexReply(int sock, TimeIndex* index);
    Status ReadTimeIndexReply(uint8_t* data, TimeIndex* &index);

    Status SendVarIndexRequest(int sock);
    Status SendVarIndexReply(int sock, VarIndex* index);
    Status ReadVarIndexReply(uint8_t* data, VarIndex* &index);

    /*bind data functions*/
    Status SendBindDataSourceRequest(int sock, std::string &SSName, StorageID &storeID);
    Status ReadBindDataSourceRequest(uint8_t* data, std::string &SSName, StorageID &storeID);
    Status SendBindDataSourceReply(int sock, bool &ret);
    Status ReadBindDataSourceReply(uint8_t* data, bool &ret);

    /*some structure*/
    flatbuffers::Offset<StoreSiteRequest> GetSite(flatbuffers::FlatBufferBuilder &fbb, StoreSite *site);
    Status SetSite(const flatbuffers::Vector<flatbuffers::Offset<StoreSiteRequest>> *siteVectorf, std::vector<StoreSite*> *siteVector);

    flatbuffers::Offset<ContentIDRequest> GetContentID(flatbuffers::FlatBufferBuilder &fbb, ContentID &cntID);
    Status SetContentID(const ContentIDRequest *cntIDf, ContentID &cntID);

    flatbuffers::Offset<ContentDescRequest> GetContentDesc(flatbuffers::FlatBufferBuilder &fbb, ContentDesc &cntDesc);
    Status SetContentDesc(const ContentDescRequest *cntDescf, ContentDesc &cntDesc);

    flatbuffers::Offset<FilePathListRequest> GetFilePathList(flatbuffers::FlatBufferBuilder &fbb, FilePathList *fileList);
    Status SetFilePathList(const FilePathListRequest *fileListf, FilePathList *fileList);

    flatbuffers::Offset<StoreDescRequest> GetStoreDesc(flatbuffers::FlatBufferBuilder &fbb, StoreDesc &storeDesc);
    Status SetStoreDesc(const StoreDescRequest *storeDescf, StoreDesc &storeDesc);

    flatbuffers::Offset<TimeListRequest> GetTimeList(flatbuffers::FlatBufferBuilder &fbb, TimeList* slot);
    Status SetTimeList(const TimeListRequest *timeListf, TimeList* &list);

    flatbuffers::Offset<TimeSlotNodeRequest> GetTimeSlotNode(flatbuffers::FlatBufferBuilder &fbb, TimeSlotNode* slot);
    Status SetTimeSlotNode(const TimeSlotNodeRequest *timeSlotNodef, TimeSlotNode* &slot);

    flatbuffers::Offset<VarListNodeRequest> GetVarListNode(flatbuffers::FlatBufferBuilder &fbb, VarListNode* node);
    Status SetVarListNode(const VarListNodeRequest *varListNode, VarListNode* &node);


};
