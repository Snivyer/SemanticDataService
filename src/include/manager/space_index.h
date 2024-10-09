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

    // 空间节点
    struct SpaceNode
    {
        size_t spaceID;                                     // 空间ID
        std::string adCode;
        std::string PSSID;                                  // 父空间ID
        struct SpaceNode* PSNode;                           // 父空间节点

        std::vector<struct SpaceNode*> CSNode;              // 子空间节点

        std::string getCompleteSpaceID(int keyLength = 3) {
            return PSSID + intToStringWithPadding(spaceID, keyLength);
        }

        SpaceNode* moveUp() {
            return this->PSNode;
        }

        SpaceNode* moveDown(size_t spaceID) {
            for(auto node: this->CSNode) {
                if(node->spaceID == spaceID) {
                    return node;
                }
            }
        }

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
        bool getResult(ResultSet &result, SpaceNode* &node);          // 解析检索结果


    
    private:
        bool getTerm(SearchTerm &term, std::string &adcode);                // 解析检索关键字

        bool getResult(ResultSet &result, int &spaceID, std::string &PSSID);  // 解析检索结果 

        bool createNode(size_t spaceID);

        bool search(std::string adcode, struct SpaceNode* &node); 
        bool insert(std::string adcode, struct SpaceNode* &node);


    };


}
