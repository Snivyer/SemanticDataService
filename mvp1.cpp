#include "app/SDS_retrieval.h"
using namespace SDS_Retrieval;
using namespace std;
using arrow::Status;


int main() {

    std::shared_ptr<SDS_Retrieval_Client> client = SDS_Retrieval_Client::createClient();

    client->connectMetaService("/tmp/meta", "");
    client->connectDBService("/tmp/store", "");
    auto ret = client->runServer();
    if(ret == 0) {
        client->disconnectMetaService();
        client->disconnectDBService();
    }

    return 0;
}