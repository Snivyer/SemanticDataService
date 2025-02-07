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
#include "abstract/IO/io.h"
#include "manager/metadata/metada_manager.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"
#include "manager/rpc/metadata_rpc/meta_protocol.h"

using arrow::Status;

namespace SDS {

    class MetaService {
        public:
            ~MetaService();
            static std::shared_ptr<MetaService> createMetaService(std::shared_ptr<EventLoop> loop);
            SemanticSpace* createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames,  MetaClient* client);
            StorageSpace* createStorageSpace(std::string spaceID, StoreTemplate &storeInfo, std::string storekind, MetaClient* client);

            
            // create the content index for local dataset
            bool createContentIndex(std::string SemanticSpaceName, std::string StoreSpaceName, std::string dirName);

            // search the content index to get content ID
            bool searchContentIndex(std::vector<std::string> geoNames, std::vector<std::string> times, std::vector<std::string> varNames, 
                                    ContentID& cntID, std::string varGroupName = "default");
            
            ContentDesc& getContentDesc(ContentID &cntID);
           
            void connectClient(int listenerSock);
            void disconnectClient(MetaClient* client);
            Status processMessage(MetaClient* client);
            void runServer();



        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit MetaService(std::shared_ptr<Impl> impl);
            bool removeClientFromSemanticSpaceEntry(SemanticSpaceEntry* entry, MetaClient* client);
            bool removeClientFromStorageSpaceEntry(StorageSpaceEntry* entry, MetaClient* client);
            bool addClientToSemanticSpaceEntry(SemanticSpaceEntry *entry, MetaClient* client);
            bool addClientToStorageSpaceEntry(StorageSpaceEntry *entry, MetaClient* client);
            bool createContentIndexInternal(std::string spaceID, size_t storageID, std::string dirName);
            
    };






}