#pragma once
#include "API/dataspace/databox_client.h"
#include "API/dataspace/meta_service_client.h"
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"

#include <unordered_map>


using namespace SDS;
using namespace std;
using arrow::Status;

namespace SDS_Retrieval {

    class SDS_Retrieval_Client {
        public:
            static std::shared_ptr<SDS_Retrieval_Client> createClient();

            /*connect_related*/ 

            Status connectDBService(const std::string& dbSocketName, const std::string& ManagerSocketName);
            Status disconnectDBService();
            Status connectMetaService(const std::string& metaSocketName, const std::string& ManagerSocketName);
            Status disconnectMetaService();
            int runServer();

            /*operations_related*/ 
            // create semantic space and storage space
            bool createSemanticStoreSpace(std::vector<std::string>& infos);

            // create semantic space and storage space in the specified bucket
            bool createByBucket(std::vector<std::string>& infos);
            
            // data import
            bool importData(std::vector<std::string>& infos);

            // data retrieval
            bool searchData(std::vector<std::string>& infos);

            // show content metadata
            void showContentInfo(std::vector<std::string>& infos);

            // get data from the data box
            bool getData(std::vector<std::string>& infos);

            // split data block according to the administrative division name, time period, and variable data block
            bool splitData(std::vector<std::string> &infos);

            /*error_related*/
            int  printError(bool flag, std::string code);
            void printOpTypeError(std::string op);
            void exitSys();
            void menu();
            

        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit SDS_Retrieval_Client(std::shared_ptr<Impl> impl);

            /* semantic space related*/
            // parse the semantic space information
            std::vector<std::string> parseSpaceInfo(std::string admin);

            // create semantic space 
            bool createSemanticSpace(std::string SSName, std::vector<std::string> geoNames);

            void showSemanticSpace();

            /* storage space related*/
            // create storage space
            bool createStorageSpace(std::string spaceID, std::string SSName, StoreTemplate &temp);
            void showStorageSpace();
    };
}




