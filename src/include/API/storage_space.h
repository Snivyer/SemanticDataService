/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   SemanticSpace is a data description set which containing content semantic of data by using  ContentID.
*/

#ifndef STORAGE_SPACE_H_
#define STORAGE_SPACE_H_

#include <string>
#include "semantic_space.h"
#include "manager/sto_meta.h"
#include "abstract/adaptor/adaptor.h"
#include "abstract/adaptor/ceph_adaptor.h"
#include "abstract/adaptor/lustre_adaptor.h"

namespace SDS
{
   static size_t globalSpaceID = 0;

    class StorageSpace
    {
    public:
        size_t storageID;          // 存储空间ID
        StorageMeta stoMeta;    // 存储元数据


        size_t createStorageSpace(struct ContentID &cntID, struct StoreTemplate &stoT);       // 创建存储空间
    
    private:
        size_t generateStorageID();                 // 生成存储空间ID
        void bindCntID(struct ContentID &cntID);    // 绑定内容ID
        bool reserveSpace(size_t spaceSize);
        
        Adaptor *adaptor;


    };

}

#endif

