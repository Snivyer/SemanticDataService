/* Author: Snivyer
*  Create on: 2024/11/18
*  Description:
    Metadata service is independent services to privoide basic management for metadata and index.
*/
#pragma once

#include <memory>
#include <arrow/api.h>
#include "arrow/util/logging.h"
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/event/events.h"
#include "abstract/utils/time_operation.h"
#include "abstract/IO/io.h"
#include "manager/metadata/metada_manager.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"
#include "manager/rpc/metadata_rpc/meta_protocol.h"
#include "API/dataspace/databox_client.h"
#include "abstract/utils/directory_operation.h"

using arrow::Status;

namespace SDS {

    class MetaService {
        public:
            ~MetaService();
            static std::shared_ptr<MetaService> createMetaService(std::shared_ptr<EventLoop> loop);
            SemanticSpace* createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames,  MetaClient* client);
            SemanticSpace* loadSemanticSpace(std::string SSName);

            StorageSpace* createStorageSpace(StoreTemplate &storeInfo, MetaClient* client);
            StorageSpace* loadStorageSpace(std::string SSName);
       

            // create the content index for local dataset
            bool createContentIndex(std::string SemanticSpaceName, std::string StoreSpaceName, std::string dirName);

            // search the content index to get content ID
            bool searchDataFile(std::string SSName, std::vector<std::string> &times, std::vector<std::string> &varNames,
                                 std::vector<FilePathList> &fileList);

            bool searchDataBox(std::string SSName, std::vector<std::string> &times, std::vector<std::string> &varNames,
                std::vector<FilePathList> &fileList, std::vector<size_t> &dbIDs);
            
            void disconnectToDataBoxService();
            void connectToDataBoxService();

            void connectClient(int listenerSock);
            void disconnectClient(MetaClient* client);
            Status processMessage(MetaClient* client);
            void runServer();



        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit MetaService(std::shared_ptr<Impl> impl);
            void init();
            bool removeClientFromSemanticSpaceEntry(SemanticSpaceEntry* entry, MetaClient* client);
            bool removeClientFromStorageSpaceEntry(StorageSpaceEntry* entry, MetaClient* client);
            bool addClientToSemanticSpaceEntry(SemanticSpaceEntry *entry, MetaClient* client);
            bool addClientToStorageSpaceEntry(StorageSpaceEntry *entry, MetaClient* client);
            bool createContentIndexInternal(std::string spaceID, size_t storageID, std::string dirName);
            bool searchData(std::string SSName, std::vector<std::string> &times, 
                std::vector<std::string> &varNames, std::vector<FilePathList> &fileList,
                std::vector<ContentID> &cntIDs, std::vector<ContentDesc> &cntDescs, std::vector<StoreDesc> &storeDescs, bool isFile = true);
            bool initDataSource();
            bool bindDataSource(std::string SSName, StorageID &storeID);
            TimeIndex* getTimeIndex();
            VarIndex* getVarIndex();
            
    };






}