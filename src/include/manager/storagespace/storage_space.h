/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   SemanticSpace is a data description set which containing content semantic of data by using  ContentID.
*/

#pragma once

#include <string>
#include "manager/semanticspace/semantic_space.h"
#include "manager/metadata/sto_meta.h"
#include "manager/metadata/cnt_meta.h"
#include "abstract/adaptor/adaptor_factory.h"
#include "abstract/utils/string_operation.h"
#include "abstract/meta/sto_ID.h"
#include "manager/index/site_index.h"



namespace SDS
{
   static size_t globalSpaceID = 0;

    struct StorageSpace {
        size_t spaceID;
        time_t createT;
        size_t adaptorNum;
        StoreDesc stoMeta;
        std::unordered_map<StorageID, Adaptor*, StorageIDHasher> adaptorIndex;     
        SpaceStatus status;

        void print() {
            std::cout << "---------------------------------" << std::endl;
            std::cout << "存储空间ID为:"  << spaceID << std::endl;
            stoMeta.print();
            std::cout << "---------------------------------" << std::endl;
        }

        void printWithTreeModel() {
            std::cout << stoMeta.SSName.data() << "(" + intToStringWithPadding(spaceID, 3) << ")" << std::endl;
            for(auto item : adaptorIndex) {
                item.first.printWithTreeModel();
                item.second->pathList->printWithTreeModel();
            }
        }

        void addAdaptor(StorageID &stoID, Adaptor* adaptor);
        Adaptor* getAdaptor(StorageID &stoID);
    };

    class  StorageSpaceManager
    {
    public:
        StorageSpaceManager();
        ~StorageSpaceManager();

        // create storage space
        size_t createStorageSpace(StoreTemplate &stoT);   
        
        // create storage index according to given dir path 
        bool createStoreTreeIndex(size_t stoID, std::string dirPath, StorageID &storeID);
  
        // bool fillDataBox(ContentDesc &cntDesc, size_t stoID, DataBox* db, size_t start, size_t count);

        // cache the storage space
        void addSpace(size_t stoID, std::string spaceName, StorageSpace* space);
        StorageSpace* getSpaceByID(size_t stoID);
        StorageSpace* getSpaceByName(std::string spaceName);


    private:
        StoreMeta *storeMeta_;
        std::map<size_t, StorageSpace*> spaceIDMap_;
        std::map<std::string, StorageSpace*> spaceNameMap_;
        std::map<size_t, Adaptor*> adapatorMap_;
        SiteIndex *siteIndex_;


        size_t generateStorageID();                 // 生成存储空间ID
        bool reserveSpace(StorageSpace *space, size_t spaceSize);
    };

}


