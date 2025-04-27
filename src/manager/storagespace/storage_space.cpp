#include "manager/storagespace/storage_space.h"


namespace SDS
{

    
    void StorageSpace::addAdaptor(StorageID &stoID, Adaptor* adaptor) {
        adaptorIndex.insert({stoID, adaptor});
    }

    Adaptor* StorageSpace::getAdaptor(StorageID &stoID) {
        auto ret = adaptorIndex.find(stoID);
        if(ret != adaptorIndex.end()) {
            return ret->second;
        }
        return nullptr;
    }

    StorageSpaceManager::StorageSpaceManager() {
        storeMeta_ = new StoreMeta();
        siteIndex_ = new SiteIndex();
        // todo： 加载存储空间

    }

    StorageSpaceManager::~StorageSpaceManager() {

        // todo: 将新增加的存储空间添加到存储系统中

        spaceIDMap_.clear();
        spaceNameMap_.clear(); 

        // todo: adaptor也将添加到对应的缓存中

        if(storeMeta_) {
            delete storeMeta_;
        }
    }

    // create storage space according to the storage template
    size_t StorageSpaceManager::createStorageSpace(StoreTemplate &stoT) {
        
        if(stoT.kind == StoreSpaceKind::None) {
            return true;
        }

        int stoID = generateStorageID();
        StorageSpace* space = new StorageSpace;
        space->spaceID = stoID;
        space->status = SpaceStatus::create;
        space->adaptorNum = 0;
        storeMeta_->extractStoreDesc(stoT, space->stoMeta);
        if(stoT.spaceSize > 0) {
            space->stoMeta.capacity = stoT.spaceSize;
            space->stoMeta.size = 0;
            reserveSpace(space, stoT.spaceSize);
        }

        addSpace(stoID, stoT.SSName, space);
        return stoID;
    }

    bool StorageSpaceManager::createStoreTreeIndex(size_t stoID, std::string dirPath, StorageID &storeID) {

        // step 1: extract site info and generate storage ID
        auto storeSpace = getSpaceByID(stoID);
        storeID.setSpaceID(intToStringWithPadding(stoID, 3));
        storeID.setTypeID(storeSpace->stoMeta.getTypeID());

        FilePathList* list = new FilePathList();
        list->extractStoreSiteDesc(dirPath);

        SearchTerm term;
        ResultSet result;
        SiteNode* node = nullptr;
        term.push_back(dirPath);

        siteIndex_->insert(term, result);
        if(siteIndex_->getResult(result, node)) {
            storeID.setSiteID(node->getCompleteSiteID());
        } else {
            storeID.setSiteID("0");
        }

        Adaptor* adaptor = AdaptorFactory::getAdaptor(storeSpace->stoMeta.kind, storeSpace->stoMeta.conConf, list);
        storeSpace->addAdaptor(storeID, adaptor);
    }


    // 生成存储空间ID
    size_t StorageSpaceManager::generateStorageID() {
        SDS::globalSpaceID ++;
        return  SDS::globalSpaceID;
    }



    // reserver space size
    bool StorageSpaceManager::reserveSpace(StorageSpace *space, size_t spaceSize) {
        // Adaptor* adaptor = getAdaptor(space->storageID);
        // space->stoMeta.sysDesc.conConf.rootPath = adaptor->AllocateSpace(spaceSize);
        return true;
    }



    void StorageSpaceManager::addSpace(size_t stoID, std::string spaceName, StorageSpace* space) {
        spaceIDMap_.insert({stoID, space});
        spaceNameMap_.insert({spaceName, space});
    }
        
    StorageSpace* StorageSpaceManager::getSpaceByID(size_t stoID) {
        auto ret = spaceIDMap_.find(stoID);
        if(ret != spaceIDMap_.end()) {
            return ret->second;
        }
        return nullptr;
    }

    StorageSpace* StorageSpaceManager::getSpaceByName(std::string spaceName) {
        auto ret = spaceNameMap_.find(spaceName);
        if(ret != spaceNameMap_.end()) {
            return ret->second;
        }
        return nullptr;
    }



    // // fill databox with the given data which described by parameter contentDesc
    // bool StorageSpaceManager::fillDataBox(ContentDesc &cntDesc, size_t stoID, DataBox* db, size_t start, size_t count) {

    //     // first step: build space index of databox
    //     // todo: 这里我不知道怎么填
    //     // SerialElementIndex spaceIndex;
    //     // spaceIndex.lastUpadatePosition = start;
    //     // spaceIndex.count = count;
    //     // db->metaSet.SpaceIndices.insert(spaceIndex);




    //     // first step: get the storage adaptor and fill data into the data region of the databox
    //     StoreMeta stoMeta;
    //     StoreDesc stoDesc = stoMeta.getStoreMeta(stoID);

    //     Adaptor* adaptor = getAdaptor(stoID);
    //     if(!adaptor) {
    //         adaptor = new Adaptor(stoDesc.sysDesc.conConf);
    //         addAdaptor(stoID, adaptor);
    //     }
        
    //     // adaptor->readVar(stoDesc.sysDesc.fileList, cntDesc.vlDesc.desc, db->data);
        
    //     // second step: generate the data index and fill it into the metaset region of the databox


    //     // third step: fill footer into the metadata region of the databox 
    // }



}
