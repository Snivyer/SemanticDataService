#include "manager/metadata/sto_meta.h"


namespace SDS
{
   
    void StoreMeta::extractStoreDesc(StoreTemplate &stoT, StoreDesc &desc)
    {
        desc.kind = stoT.kind;
        desc.writable = stoT.writable;
        desc.sysDesc.conConf = stoT.connConf;
    }

}