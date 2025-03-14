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

       
        // version 1 interface
        arrow::Status getDB(ContentID &cntID, int64_t timeout, DataboxObject *object);

        arrow::Status containDB(ContentID &cntID, bool &is_contain);

        arrow::Status releaseDB(ContentID &cntID, bool &is_release);

        arrow::Status deleteDB(ContentID &cntID, bool &is_delete);

        // version2 interface
        arrow::Status createDB(ContentID &cntID, ContentDesc &cntDesc,
                                 StoreDesc &desc, FilePathList &fileList, DBMeta& dbMeta);
        
        arrow::Status getDB(size_t dbID, int64_t timeout, DataboxObject *object);

        arrow::Status containDB(size_t dbID, bool &is_contain);

        arrow::Status releaseDB(size_t dbID, bool &is_release);

        arrow::Status deleteDB(size_t dbID, bool &is_delete);
            
        arrow::Status getContentID(size_t dbID, ContentID &cntID); 
        

        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxClient(std::shared_ptr<Impl> impl);
           

    };

  
   
    


}