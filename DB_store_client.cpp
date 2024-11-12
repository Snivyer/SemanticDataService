#include "manager/databox/databox_client.h"
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace SDS;
using namespace std;
using arrow::Status;


void runClient(ContentID &cntID, std::string datapath) {


    std::shared_ptr<DataBoxClient> client =  DataBoxClient::createClient();
    Status ret = client->connect("/tmp/store", "");

    if(ret.ok()) {
      ARROW_LOG(INFO) <<  "connect success!  \n";
    }

    ret = client->createDB(cntID, datapath);
    if(ret.ok()) {
      ARROW_LOG(INFO) <<  "databox create success! \n";
    }

    DataboxObject *dbObject;
    while(true) {
      ret = client->getDB(cntID, 0, dbObject);
      if(ret.ok()) {
        ARROW_LOG(INFO) <<  "databox get success! \n";
        break;
      } else {
        std::this_thread::sleep_for(std::chrono::seconds(10));
      }
    }

    std::this_thread::sleep_for(std::chrono::minutes(1));
    bool is_ok;
    client->releaseDB(cntID, is_ok);
    if(is_ok) {
        ARROW_LOG(INFO) <<  "databox released success! \n";
        client->deleteDB(cntID, is_ok);
        if(is_ok) {
            ARROW_LOG(INFO) <<  "databox delete success! \n";
        }
    }
    client->disconnect();
}

int main() {

    ContentID cntID1("1", "2", "3");
    std::string datapath1 = "/home/snivyer/SemanticDataService/data/ldasin/fcst/44-20230111T000000";

    ContentID cntID2("2", "2", "3");
    std::string datapath2 = "/home/snivyer/SemanticDataService/data/ldasin/fcst/33-20220620T000000";

    ContentID cntID3("1", "10", "3");
    std::string datapath3 = "/home/snivyer/SemanticDataService/data/ldasin/fcst/44-20230111T000000";

    ContentID cntID4("3", "1", "3");
    std::string datapath4 = "/home/snivyer/SemanticDataService/data/ldasin/fcst/33-20220620T000000";

    std::thread client1(runClient, std::ref(cntID1), datapath1);
    std::thread client2(runClient, std::ref(cntID2), datapath2);
    std::thread client3(runClient, std::ref(cntID3), datapath3);
    runClient(cntID4, datapath4);

    client1.join();
    client2.join();
    client3.join();
    
}

