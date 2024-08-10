/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   SemanticSpace is a data description set which containing content semantic of data by using  ContentID.
*/

#pragma once

#include <string>
#include "API/semanticspace/semantic_space.h"
#include "API/dataspace/data_box.h"
#include "manager/sto_meta.h"
#include "manager/cnt_meta.h"
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
    };

    class  StorageSpaceManager
    {
    public:
        StorageSpaceManager();
        ~StorageSpaceManager();

        
        size_t createStorageSpace(struct ContentID &cntID, struct StoreTemplate &stoT);       // 创建存储空间
        void load(size_t storeID);
        void showInfo();
        
        bool fillDataBox(ContentDesc &cntDesc, size_t stoID, DataBox* db, size_t start, size_t count );


        void addAdaptor(size_t stoID, Adaptor* adaptor);
        Adaptor* getAdaptor(size_t stoID);

        void addSpace(size_t stoID, StorageSpace* space);
        StorageSpace* getSpace(size_t stoID);


    private:

        StoreMeta *storeMeta;
        std::map<size_t, StorageSpace*> spaceMap;
        std::map<size_t, Adaptor*> adapatorMap;


        size_t generateStorageID();                 // 生成存储空间ID
        void bindCntID(struct ContentID &cntID, size_t StoID);    // 绑定内容ID
        bool reserveSpace(StorageSpace *space, size_t spaceSize);



    };

}


