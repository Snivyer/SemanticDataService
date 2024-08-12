#include "API/dataspace/data_management.h"


namespace SDS {


DataManager::DataManager() {
    semanticSpaceManager = new SemanticSpaceManager;
    storageSpaceManager = new StorageSpaceManager();
}

DataManager::~DataManager() {
    if(semanticSpaceManager) {
        delete semanticSpaceManager;
    }

    if(storageSpaceManager) {
        delete storageSpaceManager;
    }
}


bool DataManager::importData(ContentID &cntID, std::vector<std::string> filePathList, bool justMeta) {
    
    ContentMeta metaManager;
    ContentDesc cntDesc;
    metaManager.extractTSDesc(cntDesc, filePathList);


}


bool DataManager::loadData(ContentID &cntID) {

    // 初始化数据请求
    DataRequest request(cntID);
    requestNum += 1;
    request.requestID = requestNum;

    // todo: 将最新的请求添加到请求队列中
    requestQueue.push_back(request);
    return true;
}

DataBox* DataManager::getRecentDataBox() {

    if(internalLoadData()) {
        auto reponse = replyQueue.front();
        replyQueue.pop_front();
        return reponse.db;
    }  else {
        return nullptr;
    }
}

// 根据内容描述符和存储描述符创建空的数据箱子
DataBox* DataManager::createDataBox(ContentDesc &cntDesc)  {

    DataBox *db = new DataBox();
    db->init(cntDesc);
    return db;
   
}


bool DataManager::fillDataWithOneDB(SemanticSpace* space, ContentID &cntID, DataResponse &reponse, size_t start) {

    // 获取内容描述符
    ContentDesc cntDesc = space->cntDesc;

    // 获取存储描述符
    size_t stoID = cntID.getBestStoID();
    if(stoID == 0) { 
        // todo: 内容ID没有和存储ID绑定，请您绑定一下，谢谢 7/31
        return false; 
    }

    // 创建空间索引
    size_t space_count = cntDesc.vlDesc.groupLen * cntDesc.tsDesc.count;
    // todo: 添加索引的部分
    reponse.db->addSpaceIndex(start, space_count);
        
    // 以子空间的方式，填充数据箱子
    if(storageSpaceManager->fillDataBox(cntDesc, stoID, reponse.db, start, space_count) == false) {
        // todo: 将数据箱子填充失败的请求放入阻塞队列中，等待阻塞队列再次申请数据填充
        penddingQueue.push_back(reponse);
        return false;
    }

    return true;
}
        
        
        
bool DataManager::fillDataWithMultiDB(SemanticSpace* space, ContentID &CntID, DataResponse &reponse, size_t start) {

    // get children content ID by searching space index seperately
    SearchTerm term;
    ResultSet result;
    SpaceNode* sNode;

    SpaceIndex* spaceIndex = semanticSpaceManager->getSpaceIndex();
    term.push_back(space->cntDesc.ssDesc.adCode);

    if(spaceIndex->search(term, result) == false) {
        // cannot find the metadata space index
        return false;
    }

    if(spaceIndex->getResult(result, sNode) == false) {
        // cannot fetch the space node of the space index
        return false;
    }

    auto childSpaceNodes = sNode->CSNode;
    assert(childSpaceNodes.size() == space->childrenNum);

    for(auto childNode : childSpaceNodes) {
        ContentID childCntID = CntID;
        childCntID.spaceID = childNode->SpaceID;  
        
        SemanticSpace* childSpace = semanticSpaceManager->getSpaceByID(childCntID.spaceID);
        if(childSpace == nullptr) {
            // can not load the relate semantic space
            return false;
        }

        start += childSpace->cntDesc.vlDesc.groupLen * childSpace->cntDesc.tsDesc.count;
        if(fillDataWithOneDB(childSpace, childCntID, reponse, start) == false) {
            // todo:这里怎么断线重连
            return false;
        }
    }
    return true;   
}

    




// fill data to databox
bool DataManager::internallFillData(DataRequest &request, DataResponse &reponse, size_t start) {

    SemanticSpace* space = semanticSpaceManager->getSpaceByID(request.cntID.spaceID);
    if(space == nullptr) {
        // can not load the relate semantic space
        return false;
    }

    if(space->childrenNum == 0) {
        if(fillDataWithOneDB(space, request.cntID, reponse, start)) {
             // todo: 将回复消息添加到回复队列中，由回复队列将数据箱子返回给对应的客户端
            replyQueue.push_back(reponse);
            return true;
        }
    } else {
        if(fillDataWithMultiDB(space, request.cntID, reponse, start)) {
            
            // todo: 将回复消息添加到回复队列中，由回复队列将数据箱子返回给对应的客户端
            replyQueue.push_back(reponse);
            return true;
        }
    }
    return false;
}


// fetch request from the request queue, load the related data and add the reponse to the reponse queue
bool DataManager::DataManager::internalLoadData() {

    // fetch a request message from the request queue
    DataRequest request = requestQueue.front();

    // get the content description according to content ID
    ContentMeta cntMeta;
    ContentDesc cntDesc = cntMeta.getCntMeta(request.cntID);

    // create the data box with given content description
    DataBox* db = createDataBox(cntDesc);

    if(db) {
        requestQueue.pop_front();
    } else {
        return false;
    }
  
    DataResponse reponse;
    reponse.db = db;
    reponse.requestID = request.requestID;

    // start to fill data into databox
    return internallFillData(request, reponse, 0);
}


}