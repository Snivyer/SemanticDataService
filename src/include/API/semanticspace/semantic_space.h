/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   StorageSpace is a data description set which containing storage semantic of data by using ContentID.
*/

#pragma once

#include <string>
#include <map>
#include "manager/cnt_meta.h"
#include "manager/space_index.h"


namespace SDS
{

    // space stutus enum
    enum SpaceStatus {
        create,
        read,
        modify,
        remove
    };

    struct SemanticSpace {
        size_t spaceID;           // 语义空间ID
        std::string SSName;         // 语义空间名称
        std::string PSSID;           // 父语义空间ID
        size_t childrenNum;       // 子语义空间数量    
        time_t createT;             // 创建时间

        ContentID cntID;     // 内容ID描述符
        ContentDesc cntDesc;        // 内容描述符
        SpaceStatus status;         // 语义空间的状态
        SpaceNode* indexNode;

        SemanticSpace(std::string SSName) {
            this->SSName = SSName;
        }

        void init(SpaceNode *sNode, SpaceStatus state = SpaceStatus::create) {
            this->cntID.spaceID = std::to_string(sNode->spaceID);
            this->spaceID = sNode->spaceID;

            this->PSSID = sNode->PSSID;
            this->childrenNum = sNode->CSNode.size();
            this->createT = std::time(nullptr);
            this->status = state;
            this->indexNode = sNode;
        }

        std::string getCompleteSpaceID(int keyLength = 3) {
            return PSSID + intToStringWithPadding(spaceID, keyLength);
            
        }

    };

   
    class SemanticSpaceManager
    {
    public:
        SemanticSpaceManager(); 
        ~SemanticSpaceManager();

        // create the semantic space whose level number depends on the number of geoNames
        size_t createSemanticSpace(std::string SSName, std::vector<std::string> &GeoNames);
        void printSpaceDesc(SemanticSpace *space);

        SpaceIndex* getSpaceIndex();
        SemanticSpace* getSpaceByName(std::string spaceName);
        SemanticSpace* getSpaceByID(std::string spaceID);



    
    private:
        ContentMeta* _metaManager;
        SpaceIndex* _spaceIndex;
        MetaStore* _metaStore;
        std::map<std::string, SemanticSpace*> _spaceNameMap;
        std::map<std::string, SemanticSpace*> _spaceIDMap;


    };

}


