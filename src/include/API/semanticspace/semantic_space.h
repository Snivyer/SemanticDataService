/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   StorageSpace is a data description set which containing storage semantic of data by using ContentID.
*/

#pragma once

#include <string>
#include <map>
#include "manager/cnt_meta.h"


namespace SDS
{

    enum SpaceStatus {
        create,
        read,
        modify,
        remove
    };

    struct SemanticSpace {
        uint16_t spaceID;           // 语义空间ID
        std::string SSName;         // 语义空间名称
        std::string PSID;           // 父语义空间ID
        uint16_t childrenNum;       // 子语义空间数量    
        time_t createT;             // 创建时间

        struct ContentID cntID;     // 内容ID描述符
        ContentMeta cntMeta;        // 内容描述符
        SpaceStatus status;         // 语义空间的状态
    };

   
    class SemanticSpaceManager
    {
    public:
        SemanticSpaceManager(); 
        ~SemanticSpaceManager();

        int createSemanticSpace(std::string SSName, std::string GeoName);       // 创建语义空间
        int load(int spaceID);                                    // 加载语义空间
        void showInfo();                                            // 展示空间描述信息
    
    private:
        Metadata* metaManager;
        std::map<std::string, SemanticSpace> spaceMap;

    };

}


