#include "utils/directory_operation.h"
#include "abstract/adaptor/local_adaptor.h"
#include <vector>
#include <iostream>
using namespace std;
using namespace SDS;




int main() {
    std::vector<FilePathList> list;
    std::string dataPath = "./data/ldasin/ana";
    getFilePathList(dataPath, list);
    ConnectConfig connConfig;
    connConfig.rootPath = "";
    LocalAdaptor adaptor(connConfig);

    STLBuffer buffer;
    for(auto pathList: list) {
        std::vector<SDS::VarDesc> descList;
        adaptor.getVarDescList(pathList, descList);
        adaptor.readVar(pathList, descList, buffer);


        // for(auto varDesc: descList) {
        //     cout << varDesc.ncVarID << "\t" << varDesc.varName << "\n";
        //     buffer.resize(buffer.position + varDesc.varLen);
        //     adaptor.readVar(pathList, varDesc, buffer);
        // }
    }
    return 0;
}
