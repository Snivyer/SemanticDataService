#include "API/semanticspace/semantic_space.h"


namespace SDS
{

    SemanticSpaceManager::SemanticSpaceManager() {   

        metaManager = new ContentMeta;
        // toDO: 从存储系统中加载已有的语义空间
       
    }

    SemanticSpaceManager::~SemanticSpaceManager() {
        // toDO: 将新创建和修改的语义空间写回到存储系统中
    }


  



    int SemanticSpaceManager::createSemanticSpace(std::string SSName, std::string geoName)
    {

        ContentMeta cntMeta;

        // 提取空间描述符
        cntMeta.extractSSDesc(geoName);

        // 检索空间索引
    }



}