#pragma once


#include <unordered_map>
#include "arrow/api.h"
#include "arrow/util/logging.h"
#include "abstract/IO/io.h"
#include "manager/databox/databox_manager.h"
#include "manager/databox/databox_object.h"
#include "manager/rpc/metadata_rpc/db_protocol.h"

using arrow::Status;


namespace SDS
{

    struct DataboxInUseEntry {
        int count;
        DataboxObject *object;
        bool isSeal;
    };

    class DataBoxClient {
        public:
        ~DataBoxClient();
        static std::shared_ptr<DataBoxClient> createClient();
        arrow::Status connect(const std::string& storeSocketName, 
                                const std::string& managerSocketName);

        arrow::Status disconnect();

    
        // version2 interface
        arrow::Status createDB(ContentID &cntID, ContentDesc &cntDesc,
                                 StoreDesc &desc, FilePathList &fileList, DBMeta& dbMeta);
        
        arrow::Status getDB(std::vector<size_t> &ids, int64_t timeout);

        arrow::Status containDB(size_t id, bool &is_contain);

        arrow::Status releaseDB(size_t id, bool &is_release);

        arrow::Status deleteDB(std::vector<size_t> &ids, bool &is_delete);
            
        arrow::Status getContentID(size_t id, ContentID &cntID); 
        

        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxClient(std::shared_ptr<Impl> impl);
           

    };

  
   
    


}