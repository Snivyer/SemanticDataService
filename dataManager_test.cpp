#include "API/semanticspace/semantic_space.h"
using namespace std;
using namespace SDS;

// 测试创建语义空间
void createSemanticSpaceTest() {

    SemanticSpaceManager spaceManager;
    std::vector<std::string> geoNames;

    geoNames.push_back(std::string("江西省"));
    size_t spaceID1 = spaceManager.createSemanticSpace(std::string("江西省空间"), geoNames);

    geoNames.push_back(std::string("赣州市"));
    size_t spaceID2 = spaceManager.createSemanticSpace(std::string("赣州市空间"), geoNames);

    geoNames.push_back(std::string("兴国县"));
      size_t spaceID3 = spaceManager.createSemanticSpace(std::string("兴国县空间"), geoNames);

    SemanticSpace* space1 = spaceManager.getSpaceByID(std::to_string(spaceID1));
    SemanticSpace* space2 = spaceManager.getSpaceByName(std::string("兴国县空间"));
    SemanticSpace* space3 = spaceManager.getSpaceByID(std::to_string(spaceID3));

 }


int main() {
    createSemanticSpaceTest();


    return 0;
}