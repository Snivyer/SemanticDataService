#include "manager/index/var_index.h"

namespace SDS
{

    // 初始化空间索引
    VarIndex::VarIndex()
    {
        _entrance.type = IndexType::VarList;

    }

    // 析构函数
    VarIndex::~VarIndex()
    {

    }

    // 解析检索词
    bool  VarIndex::getTerm(SearchTerm &term, std::string &varName)
    {
        if(term.size() == 1) {
            varName = term[0];
            return true;
        }

        return false;
    }

    bool VarIndex::getResult(ResultSet &result, VarListNode* &nodeList) {

        if(result.size() == 1) {
            nodeList = (VarListNode*) result[0];
            return true;
        }
        
        return false;
    }


    bool VarIndex::search(std::string groupName, VarListNode *&nodeList) {
        
        auto ret = varListIndex.find(groupName);
        if(ret == varListIndex.end()) {
            return false;
        }
        return true;
    }


    bool VarIndex::search(SearchTerm &term, ResultSet &result) {
        VarListNode* nodeList = nullptr;
        std::string groupName;
        result.push_back(nodeList);
        
        if(getTerm(term, groupName) == false) {
            return false;
        }

        return search(groupName, nodeList);
    }

    bool VarIndex::insert(SearchTerm &term, ResultSet &result) {
        
        VarListNode* resultNode = nullptr;
        result.push_back(resultNode);
        std::string groupName;

        if(this->getTerm(term, groupName) == false) {
            return false;
        }

        return insert(groupName, resultNode);
       
    }

    bool VarIndex::insert(std::string groupName, VarListNode* &node) {
       
        auto ret = varListIndex.find(groupName);
        if(ret != varListIndex.end()) {
            return false;  
        }

        node = new VarListNode;

        // firstly, insert into varlistSet
        varListSet.push_back(node);
        node->varListID = varListSet.size();

        // secondly, insert into varlistIndex
        varListIndex.insert({groupName, varListSet.end()});
        return true;
    }

    bool VarIndex::remove(SearchTerm &term, ResultSet &result)
    {

    }
    
    bool VarIndex::update(SearchTerm &oldTerm, SearchTerm &newTerm, ResultSet &result)
    {

    }     
    
    bool VarIndex::persist(std::string fileName)
    {

    }
    


}