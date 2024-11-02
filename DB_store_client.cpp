#include "manager/databox/databox_client.h"

#include <iostream>

using namespace SDS;
using namespace std;
using arrow::Status;



int main() {

  ContentID cntID("1", "2", "3");
  std::string datapath = "/home/snivyer/SemanticDataService/data/ldasin/fcst/44-20230111T000000";
  // std::string datapath = "/home/snivyer/SemanticDataService/test";


  std::shared_ptr<DataBoxClient> client =  DataBoxClient::createClient();
  Status ret = client->connect("/tmp/store", "");

  if(ret.ok()) {
    ARROW_LOG(INFO) <<  "connect success!";
  }

  ret = client->createDB(cntID, datapath);
  if(ret.ok()) {
    ARROW_LOG(INFO) <<  "databox create success!";
  }

  DataboxObject *dbObject;
  ret = client->getDB(cntID, 0, dbObject);
  if(ret.ok()) {
    ARROW_LOG(INFO) <<  "databox get success!";
  }

  client->disconnect();


}

