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

    geoNames.clear();
    geoNames.push_back(std::string("湖南省"));
    size_t spaceID4 = spaceManager.createSemanticSpace(std::string("江西老表空间"), geoNames);


    // get the semantic space directly with space Name
    SemanticSpace* space2 = spaceManager.getSpaceByName(std::string("赣州市空间"));

    // get the semantic space directly with the space ID
    SemanticSpace* space4 = spaceManager.getSpaceByID(intToStringWithPadding(spaceID4, 3));

    // get the parent semantic space by browering the space index 
    SpaceNode* node = space2->indexNode->moveUp();
    SemanticSpace* space1 = spaceManager.getSpaceByID(node->getCompleteSpaceID());

    // get the children semantic space by browering the space index with local ID
    node = space2->indexNode->moveDown(spaceID3); 
    SemanticSpace* space3 = spaceManager.getSpaceByID( node->getCompleteSpaceID());

    if(space1) {
        spaceManager.printSpaceDesc(space1);
    }

    if(space2) {
        spaceManager.printSpaceDesc(space2);
    }

   if(space3) {
        spaceManager.printSpaceDesc(space3);
   }

   if(space4) {
        spaceManager.printSpaceDesc(space4);
   }

 }




int main() {
    createSemanticSpaceTest();


    return 0;
}