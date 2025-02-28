/* Author: Snivyer
*  Create on: 2023/11/14
*  Description:
   Var Index is a children class of baseIndex, which implement the var group index
   with the bidirectional list and implement the var index with the simple map.
*/


#pragma once
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "manager/index/index.h"
#include "manager/metadata/cnt_meta.h"


namespace SDS
{

  
    // variable list node
    struct VarListNode
    {
                
        /*  Name: varIndex
            key: var name
            value: var ID
        */
       std::unordered_map<std::string, size_t> varIndex;
        size_t varListID; 
        size_t varNum;

        VarListNode() {
            varNum = 0;
        }

        std::string getVarListID() {
            return std::to_string(varListID);
        }

        std::string getCompleteVarID(std::string varName) {

            if(varNum == 0) {
                return getVarListID();
            }

            auto ret = varIndex.find(varName);
            if(ret != varIndex.end()) {
                return getVarListID() + std::to_string(ret->second);
            }
            return getVarListID();
        }

        void insertVarList(std::string varName) {

            auto ret = varIndex.find(varName);
            if(ret == varIndex.end()) {
                varNum += 1;
                size_t varID = varNum;
                varIndex.insert({varName, varID});
            }
            
        }
    };



    class VarIndex : BaseIndex
    {
    public:
        VarIndex();
        ~VarIndex();

        /*  Name: varListIndex
            key: groupName of var list
            value: set of varListNode
        */
       std::unordered_map<std::string, std::vector<VarListNode*>::iterator> varListIndex;
        std::vector<VarListNode*> varListSet;

        bool search(SearchTerm &term, ResultSet &result);       // 查询节点
        bool insert(SearchTerm &term, ResultSet &result);       // 插入节点
        bool remove(SearchTerm &term, ResultSet &result);       // 移除节点
        bool update(SearchTerm &oldTerm, SearchTerm  &newTerm, ResultSet &result);       // 更新节点
        bool persist(std::string fileName); 
        bool search(std::string groupName, VarListNode* &nodeList);       
        bool insert(std::string groupName, VarListNode* &nodeList);

        /*var-realted search*/
        bool hasVar(std::string varName);

    
    private:
        bool getTerm(SearchTerm &term, std::string &varName);            // 解析检索关键字
        bool getResult(ResultSet &result, VarListNode* &nodeList);

     

    };


}
