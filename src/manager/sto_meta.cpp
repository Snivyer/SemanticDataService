#include "manager/sto_meta.h"


namespace SDS
{
    StorageMeta::StorageMeta()
    {
        this->desc = new StoreDesc();
        this->desc->capacity = 2;           // 默认2G
        this->desc->size =  0;

    }

   StorageMeta::~StorageMeta()
    {
        if(this->desc != NULL)
        {
            delete this->desc;
        }
    }

    void StorageMeta::extractStoreDesc(struct StoreTemplate &stoT)
    {
        
        this->desc->kind = stoT.kind;
        this->desc->writable = stoT.writable;
        this->desc->sysDesc.conConf = stoT.connConf;

    }

}