#pragma once
#include "API/dataspace/databox_client.h"
#include "API/dataspace/meta_service_client.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"
#include "abstract/utils/directory_operation.h"
#include "abstract/meta/cnt_ID.h"
#include <unordered_map>

using namespace SDS;
using arrow::Status;

namespace SDS_Retrieval {


    struct SpaceInfo {
        SpaceInfo* parent;
        std::string spaceID;
        std::string spaceName;    
        std::unordered_map<std::string, SpaceInfo*> children;
    };


    class SDS_Retrieval_Client {
        public:
            static std::shared_ptr<SDS_Retrieval_Client> createClient();

            /*connect_related*/ 
            Status connectDBService(const std::string& dbSocketName, const std::string& ManagerSocketName);
            Status disconnectDBService();
            Status connectMetaService(const std::string& metaSocketName, const std::string& ManagerSocketName);
            Status disconnectMetaService();

            /*server_related*/
            int runServer();

            /*operations_related*/ 
            // create semantic space and storage space
            bool createSemanticStoreSpace(std::vector<std::string>& infos);

            // load semantic space
            bool loadSemanticSpace(std::vector<std::string>& infos);

            // create semantic space and storage space in the specified bucket
            bool createByBucket(std::vector<std::string>& infos);

            // data import
            bool importData(std::vector<std::string>& infos);

            // data export
            bool exportData(std::vector<std::string>& infos);

            // data retrieval
            bool searchData(std::vector<std::string>& infos);

            // databox retrieval
            bool searchDataBox(std::vector<std::string>& infos);

            // show content metadata
            void showContentInfo(std::vector<std::string>& infos);
            void detailContentInfo(std::vector<std::string>& infos);

            // get data from the data box
            bool getData(std::vector<std::string>& infos);

            // split data block according to the administrative division name, time period, and variable data block
            bool splitData(std::vector<std::string> &infos);

            /*error_related*/
            int  printError(bool flag, std::string code);
            void printOpTypeError(std::string op);
            void exitSys();

            /*help_related*/
            void menu();
            

        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit SDS_Retrieval_Client(std::shared_ptr<Impl> impl);

            /* semantic space related*/
      
            // create semantic space 
            bool createSemanticSpace(std::string SSName, std::vector<std::string> geoNames);

            // load semantic space
            bool loadSemanticSpace(std::string SSName);

            void cacheSemanticSpace(SemanticSpace &space); 
            bool addToSpaceTree(SemanticSpace &space);
            bool addToSpaceTree(std::string PSSID, SpaceInfo* info, int keyLength = 3);
        
            // show semantic space
            void showSemanticSpace(std::string SSName = "*");
            void detailSemanticSpace(std::string SSName, std::string model = "table");

            /* storage space related*/
            // create storage space
            bool createStorageSpace(std::string spaceID, std::string SSName, StoreTemplate &temp);

            // show storage space
            void showStorageSpace(std::string ssName);

            /*search related*/
            bool searchDataFile(std::string SSName, std::vector<std::string> &times, std::vector<std::string> &varNames);
            bool searchDataBox(std::string SSName, std::vector<std::string> &times, std::vector<std::string> &varNames);
            

            /*show search result*/
            bool showSearchResult(std::string spaceName, std::string spaceID);

            /* databox related*/
            bool showDBInfo();

            /*present related*/
            bool printSemanticSpaceWithTreeView(std::string spaceID = "001");
            bool printSemanticSpaceWithTreeView(SpaceInfo* info, int level = 0);

            // bool printStorageSpaceWithTreeView(std::string spaceID = "001");
            // bool printStorageSpaceWithTreeView(SpaceInfo* info, int level = 0);

            /*data export*/
            bool exportFile(std::string destPath);




    };
}




