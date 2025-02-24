/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   StorageSpace is a data description set which containing storage semantic of data by using ContentID.
*/

#pragma once

#include <string>
#include <unordered_map>
#include "abstract/meta/cnt_ID.h"
#include "abstract/meta/sto_meta_template.h"
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

    struct  SemanticSpace {
        size_t spaceID; 
        SSDesc ssDesc;          
        std::string SSName;         
        std::string PSSID;          
        size_t childrenNum;          
        time_t createT;             
        size_t databoxNum;  
        
        std::unordered_map<ContentID, ContentDesc, ContentIDHasher> databoxsIndex;
        SpaceStatus status;         
        SpaceNode* indexNode;

        SemanticSpace() {}

        SemanticSpace(std::string SSName) {
            this->SSName = SSName;
        }

        

        void init(SpaceNode *sNode, SpaceStatus state = SpaceStatus::create) {
            this->spaceID = sNode->spaceID;
            this->PSSID = sNode->PSSID;
            this->childrenNum = sNode->CSNode.size();
            this->createT = std::time(nullptr);
            this->status = state;
            this->indexNode = sNode;
            this->databoxNum = 0;
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
            std::cout <<  "创建时间:" << std::put_time(std::localtime(&createT), "%Y-%m-%d %H:%M:%S") << std::endl;
            std::cout << "数据箱子数量为:" << databoxNum << std::endl;
            ssDesc.print();

            for(auto item : databoxsIndex) {
                std::cout << "---------------------------------" << std::endl;
                item.first.print();
                item.second.print();
            }
            std::cout << "---------------------------------" << std::endl;
        }

        void printWithTreeModel() {
            std::cout << SSName.data() << "(" + getCompleteSpaceID() << ")" << std::endl;
            for(auto item : databoxsIndex) {
                item.first.printWithTreeModel();
                item.second.printWithTreeModel();
            }

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

        // add a data box index into the semantic space
        bool createDataBoxIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath);

        // auto refer to databox from children space
        void autoReferDataBox(SemanticSpace* space);

        // auto add databox to parent space
        bool autoAddDataBox(SemanticSpace* space, const ContentID &cntID, ContentDesc &cntdesc);



    
    private:
        ContentMeta* _metaManager;
        SpaceIndex* _spaceIndex;
        TimeIndex* _timeIndex;
        VarIndex* _varIndex;
        MetaStore* _metaStore;
        std::map<std::string, SemanticSpace*> _spaceNameMap;
        std::map<std::string, SemanticSpace*> _spaceIDMap;

        // create time index
        bool createTimeIndex(Adaptor* adaptor, SemanticSpace* space, std::string dirPath,
            ContentID &cntID, TSDesc &desc);

        // create var index 
        bool createVarIndex(Adaptor* adaptor, SemanticSpace* space, std::string dirPath, 
            ContentID &cntID, VLDesc &desc);



    };

}


