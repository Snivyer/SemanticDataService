/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
    StorageMetadata is a children class of Metadata class, which is rensponsible for management
    of the storage metadata and construction of address index.
*/

#pragma once

#include "meta.h"
#include "abstract/meta/sto_meta_template.h"
#include <string>
#include <vector>

namespace SDS
{

    // 存储元数据管理类
    class StoreMeta : public Metadata
    {
    public:
  
        /*------提取元数据------*/
        void extractStoreDesc(StoreTemplate &stoT, StoreDesc &desc);      
      

        /*------获取元数据------*/
        StoreDesc getStoreMeta(size_t StoreID);

        
        void putMeta(Key key, std::string value) override;
        bool putMetaWithJson(std::string path) override;
        bool putMetaWithCSV(std::string path) override;
        bool putMetaWithParquet(std::string path) override;
        std::string getMeta(Key key) override;
        
    private:
          



    };


}



