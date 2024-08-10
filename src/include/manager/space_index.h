/* Author: Snivyer
*  Create on: 2023/11/14
*  Description:
   Space Index is a children class of baseIndex, which implement the space index with 
   multi-tree.
*/


#pragma once
#include <string>
#include <vector>
#include <queue>
#include "index.h"
#include "manager/cnt_meta.h"


namespace SDS
{
    // 空间描述符表
    struct SpaceDescList
    {
        size_t SpaceID;
        SSDesc desc;
    };

    // 空间节点
    struct SpaceNode
    {
        size_t SpaceID;                                     // 空间ID
        struct SpaceDescList* me;                           // 空间描述符
        std::string PSSID;                                  // 父空间ID
        struct SpaceDescList* parent;                       // 父空间描述符
        struct SpaceNode* PSNode;                           // 父空间节点

        std::vector<struct SpaceDescList>* children;         // 子空间描述符表
        std::vector<struct SpaceNode*> CSNode;              // 子空间节点
    };

  class SpaceIndex : public BaseIndex
    {
    public:
        SpaceIndex();
        ~SpaceIndex();
        
        bool search(SearchTerm &term, ResultSet &result) override;       // 查询节点
        bool insert(SearchTerm &term, ResultSet &result) override;       // 插入节点
        bool remove(SearchTerm &term, ResultSet &result) override;       // 移除节点
        bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result) override;       // 更新节点
        bool persist(std::string fileName) override; 
        bool getResult(ResultSet &result, struct SpaceNode *node);          // 解析检索结果
    
    private:
        bool getTerm(SearchTerm &term, std::string &adcode);                // 解析检索关键字

        bool getResult(ResultSet &result, int &spaceID, std::string &PSSID);  // 解析检索结果 

        bool createNode(int SpaceID);

        bool search(std::string adcode, struct SpaceNode* &node); 
        bool insert(std::string adcode, struct SpaceNode *node);
        bool insert(std::string adcode, SpaceDescList* &me, std::string PSSID = std::string(0));

    };


}
