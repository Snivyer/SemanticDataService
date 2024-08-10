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
        ContentDesc &cntDesc;        // 内容描述符
        SpaceStatus status;         // 语义空间的状态

        SemanticSpace(ContentDesc &cntDesc, SpaceNode *sNode, std::string SSName): cntDesc(cntDesc) {
            cntID.spaceID = sNode->SpaceID;
            spaceID = sNode->SpaceID;
            SSName = SSName;
            PSSID = sNode->PSSID;
            childrenNum = sNode->CSNode.size();
            createT = std::time(nullptr);
            status = SpaceStatus::create;
        }

    };

   
    class SemanticSpaceManager
    {
    public:
        SemanticSpaceManager(); 
        ~SemanticSpaceManager();

        // create the semantic space whose level number depends on the number of geoNames
        size_t createSemanticSpace(std::string SSName, std::vector<std::string> &GeoNames);
        

        SpaceIndex* getSpaceIndex();
        SemanticSpace* getSpaceByName(std::string spaceName);
        SemanticSpace* getSpaceByID(std::string spaceID);

    
    private:
        ContentMeta* _metaManager;
        SpaceIndex* _spaceIndex;
        std::map<std::string, SemanticSpace*> _spaceNameMap;
        std::map<std::string, SemanticSpace*> _spaceIDMap;


    };

}


