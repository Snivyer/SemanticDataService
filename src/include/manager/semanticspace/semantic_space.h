/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   StorageSpace is a data description set which containing storage semantic of data by using ContentID.
*/

#pragma once

#include <string>
#include <map>
#include "manager/metadata/cnt_meta.h"
#include "manager/index/space_index.h"
#include "manager/index/time_index.h"
#include "manager/index/var_index.h"

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

        SemanticSpace() {}

        SemanticSpace(std::string SSName) {
            this->SSName = SSName;
        }

        

        void init(SpaceNode *sNode, SpaceStatus state = SpaceStatus::create) {
            this->cntID.setSpaceID( sNode->getCompleteSpaceID());
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

        void print() {
            std::cout << "---------------------------------" << std::endl;
            std::cout << "语义空间名为:"  << SSName.data() << std::endl;
            std::cout << "语义空间ID为:" << getCompleteSpaceID().data() << std::endl;
            std::cout << "父语义空间ID为:" << PSSID.data() << std::endl;
            std::cout << "子语义空间数量为:" << childrenNum << std::endl;
            std::cout <<  "创建时间" << std::put_time(std::localtime(&createT), "%Y-%m-%d %H:%M:%S") << std::endl;
            cntID.print();
            cntDesc.print();
            std::cout << "---------------------------------" << std::endl;
        }

    };

   
    class SemanticSpaceManager
    {
    public:
        SemanticSpaceManager(); 
        ~SemanticSpaceManager();

        // create the semantic space whose level number depends on the number of geoNames
        std::string createSemanticSpace(std::string SSName, std::vector<std::string> &GeoNames);
        void printSpaceDesc(SemanticSpace *space);


        ContentMeta* getContentMeta();
        SpaceIndex* getSpaceIndex();
        TimeIndex* getTimeIndex();
        VarIndex* getVarIndex();


        SemanticSpace* getSpaceByName(std::string spaceName);
        SemanticSpace* getSpaceByID(std::string spaceID);


        // create time index
        bool createTimeIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath);

        // create var index 
        bool createVarIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath, std::string varGroupName = "default");
            



    
    private:
        ContentMeta* _metaManager;
        SpaceIndex* _spaceIndex;
        TimeIndex* _timeIndex;
        VarIndex* _varIndex;
        MetaStore* _metaStore;
        std::map<std::string, SemanticSpace*> _spaceNameMap;
        std::map<std::string, SemanticSpace*> _spaceIDMap;


    };

}


