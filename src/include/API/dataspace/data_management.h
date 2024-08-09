/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   data management class is to implement the basic operations of semantic data service
*/

#pragma once

#include <string>
#include <vector>
#include <deque>
#include "manager/cnt_meta.h"
#include "manager/sto_meta.h"
#include "API/dataspace/data_box.h"
#include "API/semanticspace/semantic_space.h"
#include "API/storagespace/storage_space.h"

namespace SDS
{

    struct DataRequest {
        size_t requestID;
        ContentID &cntID;

        DataRequest(ContentID &cntID): cntID(cntID) {};
    };

    struct DataResponse {
        size_t requestID;
        DataBox* db;

    };

    class DataManager
    {
    public:
        DataManager();
        ~DataManager();


        bool loadSSByID(std::string spaceID);
        bool loadSSByName(std::string geoName);

        bool importData(std::string spaceID, std::vector<std::string> filePathList, bool justMeta);

        bool loadData(ContentID &cntID);
        DataBox* getRecentDataBox();
        
        


    private:
        SemanticSpaceManager* semanticSpaceManager;
        StorageSpaceManager* storageSpaceManager;
        int requestNum;


        std::map<std::string, std::string> geoName2SID;
        std::map<std::string, ContentMeta> SID2CntMeta;
        std::map<std::string, std::vector<ContentID>> SID2CntID;


        std::deque<DataRequest>  requestQueue;
        std::deque<DataResponse> replyQueue;
        std::deque<DataResponse> penddingQueue;
        bool internalLoadData();

        DataBox* createDataBox(ContentDesc &cntDesc);


    };



}
   