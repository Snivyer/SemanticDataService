#pragma once


#include <unordered_map>
#include "arrow/api.h"
#include "arrow/util/logging.h"
#include "abstract/IO/io.h"
#include "manager/rpc/metadata_rpc/meta_protocol.h"
#include "abstract/meta/sto_meta_template.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"

using arrow::Status;


namespace SDS
{

    class MetaServiceClient {
        public:
        ~MetaServiceClient();
        static std::shared_ptr<MetaServiceClient> createClient();
        arrow::Status connect(const std::string& storeSocketName, 
                                const std::string& managerSocketName);

        arrow::Status disconnect();
        
        arrow::Status createSemanticSpace(std::string ssName, std::vector<std::string> &geoNames, SemanticSpace &space);
        
        arrow::Status loadSemanticSpace(std::string ssName, SemanticSpace &space);
    

        arrow::Status createStorageSpace(std::string spaceID, std::string ssName, StoreTemplate &temp, StorageSpace &space);
       
        arrow::Status importDataFromLocal(std::string semanticSpace, std::string storageSpace, std::string dirPath);
       
        arrow::Status searchContentIndex(std::vector<std::string> &geoNames, std::vector<std::string> &times,
                                        std::vector<std::string> &varNames, ContentID &cntID, std::string groupName = "default");
        
        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit MetaServiceClient(std::shared_ptr<Impl> impl);

    };

  
   
    


}