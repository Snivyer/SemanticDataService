#include "API/storage_space.h"


namespace SDS
{

    size_t  StorageSpace::createStorageSpace(struct ContentID &cntID, struct StoreTemplate &stoT)
    {

        this->storageID = generateStorageID();
        this->bindCntID(cntID);
        
        // 无需创建存储空间，直接返回
        if(stoT.kind == StoreSpaceKind::None)
        {
            return true;
        }

        // 提取存储描述符
        this->stoMeta.extractStoreDesc(stoT);

        // 如果需要预留空间，则需要申请
        if(stoT.spaceSize > 0)
        {
            this->stoMeta.desc->capacity = stoT.spaceSize;
            this->stoMeta.desc->size = 0;

            this->reserveSpace(stoT.spaceSize);
        }

        // 初始化连接符并尝试连接
        if(this->stoMeta.desc->kind == StoreSpaceKind::Ceph)
        {
            this->adaptor = new CephAdaptor(this->stoMeta.desc->sysDesc.conConf);
        }
        else if (this->stoMeta.desc->kind == StoreSpaceKind::Lustre)
        {
            this->adaptor = new LustreAdaptor(this->stoMeta.desc->sysDesc.conConf);
        }
        else
        {
            return false;
        }


        if (this->adaptor->connect() == false)
        {
            return false;
        } 

    }

    // 生成存储空间ID
    size_t StorageSpace::generateStorageID()
    {
        SDS::globalSpaceID ++;
        return  SDS::globalSpaceID;
    }


     // 绑定存储空间ID
    void StorageSpace::bindCntID(struct ContentID &cntID)
    {
        cntID.StorageID.push_back(this->storageID);
    }


    // 预留空间
    bool StorageSpace::reserveSpace(size_t spaceSize)
    {
        // 分配空间
        this->stoMeta.desc->sysDesc.conConf.rootPath = adaptor->AllocateSpace(spaceSize);
    }





}
