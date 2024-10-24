#include "abstract/utils/directory_operation.h"
#include "abstract/adaptor/local_adaptor.h"
#include "manager/databox/databox_object.h" 
#include <vector>
#include <iostream>
using namespace std;
using namespace SDS;




int main() {
    std::vector<FilePathList> list;
    std::string dataPath = "./data/ldasin/fcst/44-20230111T000000";
    getFilePathList(dataPath, list);
    ConnectConfig connConfig;
    connConfig.rootPath = "";

    std::shared_ptr<Adaptor> adatpor(new LocalAdaptor(connConfig));
    DataboxObject dbObject;

    for(auto pathList: list) {
        dbObject.setDataPath(pathList);
        dbObject.fillData(adatpor);
        dbObject.print();
    }
    return 0;
}
