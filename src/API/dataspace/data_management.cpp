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
    db->Init(cntDesc);
    return db;
   
}




bool DataManager::DataManager::internalLoadData() {

    // 从请求队列中拿出消息进程处理
    auto request = requestQueue.front();

    // 获取内容描述符
    ContentMeta cntMeta;
    ContentDesc cntDesc = cntMeta.getCntMeta(request.cntID);

    // 获取存储描述符
    size_t stoID = request.cntID.getBestStoID();
    if(stoID == 0) { 
        // todo: 内容ID没有和存储ID绑定，请您绑定一下，谢谢 7/31
        return false; 
    }

    // 创建数据箱子
    DataBox* db = createDataBox(cntDesc);

    if(db) {
        requestQueue.pop_front();
    } else {
        return false;
    }
  

    DataResponse reponse;
    reponse.db = db;
    reponse.requestID = request.requestID;
    
    // 填充数据箱子
    if(storageSpaceManager->fillDataBox(cntDesc, stoID, db)) {
        // todo: 将回复消息添加到回复队列中，由回复队列将数据箱子返回给对应的客户端
        replyQueue.push_back(reponse);
        return true;
    } else {
        // todo: 将数据箱子填充失败的请求放入阻塞队列中，等待阻塞队列再次申请数据填充
        penddingQueue.push_back(reponse);
        return false;
    }
    
}


}