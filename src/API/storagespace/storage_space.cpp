#include "API/storagespace/storage_space.h"


namespace SDS
{

    StorageSpaceManager::StorageSpaceManager() {
        storeMeta = new StoreMeta();
        // todo： 加载存储空间

    }

    StorageSpaceManager::~StorageSpaceManager() {

        // todo: 将新增加的存储空间添加到存储系统中

        spaceMap.clear();

        if(storeMeta) {
            delete storeMeta;
        }
    }

    // create storage space according to the storage template
    size_t  StorageSpaceManager::createStorageSpace(struct ContentID &cntID, struct StoreTemplate &stoT)
    {
        // step1: generate the storage ID
        int stoID = generateStorageID();
        this->bindCntID(cntID, stoID);

   
        // 无需创建存储空间，直接返回
        if(stoT.kind == StoreSpaceKind::None)
        {
            return true;
        }

        // step2: create the storage space
        StorageSpace* space = new StorageSpace;
        space->storageID = stoID;
        space->status = SpaceStatus::create;

        // 提取存储描述符
        storeMeta->extractStoreDesc(stoT, space->stoMeta);

        // 如果需要预留空间，则需要申请
        if(stoT.spaceSize > 0)
        {
            space->stoMeta.capacity = stoT.spaceSize;
            space->stoMeta.size = 0;
            reserveSpace(space, stoT.spaceSize);
        }

        // 初始化连接符并尝试连接
        if(space->stoMeta.kind == StoreSpaceKind::Ceph) {
           // this->adaptor = new CephAdaptor(this->stoMeta->desc->sysDesc.conConf);
        }
        else if (space->stoMeta.kind == StoreSpaceKind::Lustre) {
           // this->adaptor = new LustreAdaptor(this->stoMeta->desc->sysDesc.conConf);
        }
        else {
            Adaptor* adaptor = new LocalAdaptor(space->stoMeta.sysDesc.conConf);
            addAdaptor(stoID, adaptor);
        }
    }

    // 生成存储空间ID
    size_t StorageSpaceManager::generateStorageID()
    {
        SDS::globalSpaceID ++;
        return  SDS::globalSpaceID;
    }


     // 绑定存储空间ID
    void StorageSpaceManager::bindCntID(struct ContentID &cntID, size_t stoID)
    {
        cntID.storeIDs.push_back(stoID);
    }


    // reserver space size
    bool StorageSpaceManager::reserveSpace(StorageSpace *space, size_t spaceSize)
    {
        Adaptor* adaptor = getAdaptor(space->storageID);
        space->stoMeta.sysDesc.conConf.rootPath = adaptor->AllocateSpace(spaceSize);
        return true;
    }

    // 将存储空间ID和存储适配器添加到映射表中
    void StorageSpaceManager::addAdaptor(size_t stoID, Adaptor* adaptor) {
        adapatorMap.insert({stoID, adaptor});
    }

    // 根据存储ID获取对应的存储适配器
    Adaptor*  StorageSpaceManager::getAdaptor(size_t stoID) {
        auto ret = adapatorMap.find(stoID);
        if(ret != adapatorMap.end()) {
            return ret->second;
        }
        return nullptr;
    }

    void StorageSpaceManager::addSpace(size_t stoID, StorageSpace* space) {
        spaceMap.insert({stoID, space});
    }
        
    StorageSpace* StorageSpaceManager::getSpace(size_t stoID) {
        auto ret = spaceMap.find(stoID);
        if(ret != spaceMap.end()) {
            return ret->second;
        }
        return nullptr;
    }



    // fill databox with the given data which described by parameter contentDesc
    bool StorageSpaceManager::fillDataBox(ContentDesc &cntDesc, size_t stoID, DataBox* db, size_t start, size_t count) {

        // first step: build space index of databox
        // todo: 这里我不知道怎么填
        SerialElementIndex spaceIndex;
        spaceIndex.lastUpadatePosition = start;
        spaceIndex.count = count;
        db->metaSet.SpaceIndices.insert(spaceIndex);




        // first step: get the storage adaptor and fill data into the data region of the databox
        StoreMeta stoMeta;
        StoreDesc stoDesc = stoMeta.getStoreMeta(stoID);

        Adaptor* adaptor = getAdaptor(stoID);
        if(!adaptor) {
            adaptor = new Adaptor(stoDesc.sysDesc.conConf);
            addAdaptor(stoID, adaptor);
        }
        
        adaptor->readVar(stoDesc.sysDesc.fileList, cntDesc.vlDesc.desc, db->data);
        
        // second step: generate the data index and fill it into the metaset region of the databox


        // third step: fill footer into the metadata region of the databox 




    }



}
