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
#include "index.h"
#include "manager/cnt_meta.h"


namespace SDS
{
    // 变量ID
    using VarUID = std::vector<int>;

    // 变量列表节点
    struct VarListNode
    {
        size_t varListID;                                  // 变量列表ID
        struct VLDesc* me;                                // 变量列表描述符  
        std::unordered_map<std::string, int> varMap;     //  变量名索引表

        size_t frontID;                                 // 前一个变量列表ID
        struct VarListNode* frontNode;                  // 前一个节点

        size_t behindID;                                // 后一个变量列表ID
        struct VarListNode* behindNode;                 // 后一个节点 
    };


    class VarIndex : BaseIndex
    {
    public:
        VarIndex(struct IndexEntrance &entrance);
        ~VarIndex();

        std::unordered_map<std::string, std::vector<std::string>> globalVarMap;
        std::unordered_map<std::string, struct VarDesc*>   varDescMap;
        
        bool search(SearchTerm &term, ResultSet &result);       // 查询节点
        bool insert(SearchTerm &term, ResultSet &result);       // 插入节点
        bool remove(SearchTerm &term, ResultSet &result);       // 移除节点
        bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result);       // 更新节点
        bool persist(std::string fileName); 

    
    private:
        bool getTerm(SearchTerm &term, std::string &varName);            // 解析检索关键字
        //bool getResult(ResultSet &result);   // 解析检索结果 
        
                        

    };


}
