#include "API/semanticspace/semantic_space.h"


namespace SDS
{

    SemanticSpaceManager::SemanticSpaceManager() {   

        _metaManager = new ContentMeta();
        _spaceIndex = new SpaceIndex();

        // toDO: 从存储系统中加载已有的索引结构

        // toDO: 从存储系统中加载已有的语义空间

    }

    SemanticSpaceManager::~SemanticSpaceManager() {
        // toDO: 将新创建和修改的语义空间写回到存储系统中

        // toDO: 将更新后的索引结构更新回存储系统中
    }


    // create a semantic space
    size_t SemanticSpaceManager::createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames)
    {
        ContentDesc cntDesc;
        _metaManager->extractSSDesc(cntDesc, geoNames);

        // choose the adimistrator code as the search term
        SearchTerm term;
        ResultSet result;
        term.push_back(cntDesc.ssDesc.adCode);


        SpaceNode* node;
        // search space index
        if(_spaceIndex->search(term, result)) {
            if(_spaceIndex->getResult(result, node)) {
                return node->SpaceID;
            }
            
        } else {
            if(_spaceIndex->getResult(result, node)) {
                // insert into space index and crate the new semantic space
                if(_spaceIndex->insert(term, result)) {
                    if(_spaceIndex->getResult(result, node)) {

                        // create a semantic space
                        SemanticSpace* space = new SemanticSpace(cntDesc, node, SSName);
                        _spaceNameMap.insert({SSName, space});
                        _spaceIDMap.insert({node->SpaceID, space});
                        return node->SpaceID;
                    }
                }
            }
        }
    }
     

    SpaceIndex* SemanticSpaceManager::getSpaceIndex() {
        return _spaceIndex;
    }


    SemanticSpace* SemanticSpaceManager::getSpaceByName(std::string spaceName) {
        auto ret = _spaceNameMap.find(spaceName);
        if(ret != _spaceNameMap.end()) {
            return ret->second;
        }
        return nullptr;
    }
    
    SemanticSpace* SemanticSpaceManager::getSpaceByID(std::string spaceID) {
        auto ret = _spaceIDMap.find(spaceName);
        if(ret != _spaceIDMap..end()) {
            return ret->second;
        }
        return nullptr;
    }
    



}