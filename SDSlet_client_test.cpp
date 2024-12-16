#include "API/dataspace/databox_client.h"
#include "API/dataspace/meta_service_client.h"
#include "manager/rpc/metadata_rpc/index_reader.h"
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace SDS;
using namespace std;
using arrow::Status;

int main() {

  // new a meta service client
  auto client = MetaServiceClient::createClient();
  Status ret = client->connect("/tmp/meta", "");
  if(ret.ok()) {
      ARROW_LOG(INFO) <<  "meta client connect success!  \n";
  }

  // firstly, create semantic space
  std::vector<std::string> geoNames;
  std::string spaceID;
  geoNames.push_back("江西省");
  geoNames.push_back("赣州市");
  geoNames.push_back("兴国县");

  
  client->createSemanticSpace("语义空间1", geoNames, spaceID);

  if(spaceID != "") {
    ARROW_LOG(INFO) <<  "semantic space create, its spaceID is " << spaceID;
  }

  // secondly, create storage space
  StoreTemplate temp;
  temp.SSName = "存储空间1";
  temp.spaceSize = 100;
  temp.writable = true;
  temp.kind = StoreSpaceKind::BB;
  temp.connConf.rootPath = "/home/snivyer/SemanticDataService/data/ldasin/fcst/";
  temp.connConf.poolName = "";
  temp.connConf.userName = "";
  temp.connConf.confFile = "";
  std::string storageID;
  client->createStorageSpace(spaceID, "存储空间1", temp, storageID);

  if(storageID != "") {
    ARROW_LOG(INFO) <<  "semantic space create, its storageID is " << storageID;
  }

  // thirdly, create content index 
  client->importDataFromLocal("语义空间1", "存储空间1", "/33-20220620T000000");

  // finally, search content index
  std::vector<std::string> times;
  std::vector<std::string> varNames;
  ContentID cntID;
  client->searchContentIndex(geoNames, times, varNames, cntID);

  ARROW_LOG(INFO) <<  "Get ContentID: spaceID:" << cntID.getSpaceID() 
                  << " TimeID:" << cntID.getTimeID() << " VarID:" << cntID.getVarID();

  client->disconnect();

  std::shared_ptr<DataBoxClient> dbClient =  DataBoxClient::createClient();
  ret = dbClient->connect("/tmp/store", "");

  if(ret.ok()) {
      ARROW_LOG(INFO) <<  "db client connect success!  \n";
  }

  ret = dbClient->createDB(cntID, "/home/snivyer/SemanticDataService/data/ldasin/fcst/33-20220620T000000");
  if(ret.ok()) {
    ARROW_LOG(INFO) <<  "databox create success! \n";
  }

  DataboxObject *dbObject;
  while(true) {
    ret = dbClient->getDB(cntID, 0, dbObject);
    if(ret.ok()) {
        ARROW_LOG(INFO) <<  "databox get success! \n";
        break;
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  }

  std::this_thread::sleep_for(std::chrono::minutes(1));
  bool is_ok;
  dbClient->releaseDB(cntID, is_ok);
  if(is_ok) {
      ARROW_LOG(INFO) <<  "databox released success! \n";
      dbClient->deleteDB(cntID, is_ok);
      if(is_ok) {
          ARROW_LOG(INFO) <<  "databox delete success! \n";
      }
  }
  dbClient->disconnect();

}

