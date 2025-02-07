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
#include "abstract/adaptor/adaptor.h"
#include "abstract/adaptor/ceph_adaptor.h"
#include "abstract/adaptor/lustre_adaptor.h"
#include "abstract/adaptor/local_adaptor.h"



namespace SDS
{
   static size_t globalSpaceID = 0;

    struct StorageSpace {
        size_t storageID;          // 存储空间ID
        StoreDesc stoMeta;        // 存储元数据
        SpaceStatus status;

        void print() {
            std::cout << "---------------------------------" << std::endl;
            std::cout << "存储空间ID为:"  << storageID << std::endl;
            stoMeta.print();
            std::cout << "---------------------------------" << std::endl;
        }
    };

    class  StorageSpaceManager
    {
    public:
        StorageSpaceManager();
        ~StorageSpaceManager();

        // create storage space
        size_t createStorageSpace(StoreTemplate &stoT);      
  
        // bool fillDataBox(ContentDesc &cntDesc, size_t stoID, DataBox* db, size_t start, size_t count);


        void addAdaptor(size_t stoID, Adaptor* adaptor);
        Adaptor* getAdaptor(size_t stoID);

        void addSpace(size_t stoID, std::string spaceName, StorageSpace* space);
        StorageSpace* getSpaceByID(size_t stoID);
        StorageSpace* getSpaceByName(std::string spaceName);


    private:
        StoreMeta *storeMeta;
        std::map<size_t, StorageSpace*> spaceIDMap_;
        std::map<std::string, StorageSpace*> spaceNameMap_;
        std::map<size_t, Adaptor*> adapatorMap_;


        size_t generateStorageID();                 // 生成存储空间ID
        void bindCntID(struct ContentID &cntID, size_t StoID);    // 绑定内容ID
        bool reserveSpace(StorageSpace *space, size_t spaceSize);
    };

}


