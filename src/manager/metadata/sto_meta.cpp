#include "manager/metadata/sto_meta.h"


namespace SDS
{
   
    void StoreMeta::extractStoreDesc(StoreTemplate &stoT, StoreDesc &desc) {
        desc.SSName = stoT.SSName;
        desc.kind = stoT.kind;
        desc.writable = stoT.writable;
        desc.sysDesc.conConf = stoT.connConf;
        desc.sysDesc.fileList.dirPath = stoT.connConf.rootPath;
    }


    StoreDesc StoreMeta::getStoreMeta(size_t StoreID) {

    }

    void StoreMeta::putMeta(Key key, std::string value) {

    }

    bool StoreMeta::putMetaWithJson(std::string path) {

    }

    bool StoreMeta::putMetaWithCSV(std::string path) {

    }

    bool StoreMeta::putMetaWithParquet(std::string path) {

    }

    std::string StoreMeta::getMeta(Key key) {
        
    }

}