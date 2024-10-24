#include "manager/index/space_index.h"
#include <iostream>

namespace SDS {

    // 初始化空间索引
    SpaceIndex::SpaceIndex() {
        // 声明索引类型为空间索引
        SpaceNode *node = new SpaceNode;
        node->spaceID = 0;
        _entrance.type = IndexType::Space;
        _entrance.rootNode = node;
        _entrance.nodeNum = 0;
    }

    // 析构函数
    SpaceIndex::~SpaceIndex() {
    
        if(this->_entrance.rootNode != NULL) {
            SpaceNode* node = (SpaceNode*) _entrance.rootNode;

            // 构建解析队列
            std::queue<struct SpaceNode*> destructorList;       
            destructorList.push(node);
            

            while(destructorList.empty() == false) {
                node = destructorList.front();
                destructorList.pop();

                std::vector<SpaceNode*>::iterator itBegin = node->CSNode.begin();
                std::vector<SpaceNode*>::iterator itEnd = node->CSNode.end();

                // 依次将子节点加入到析构队列中，完成空间释放
                while(itBegin != itEnd) {
                    destructorList.push(*itBegin);
                    itBegin++;
                }

                // std::cout << "析构节点的行政编码为:" << node->adCode  << "节点！" << std::endl; 
                delete node;
                  
            }
        }  
    }

    // 解析检索词
    bool SpaceIndex::getTerm(SearchTerm &term, std::string &adcode) {
       
        if(term.size() == 1) {
            // 使用行政编码作为检索词
            adcode = term[0];
            return true;
        }

        return false;
    }

    // 解析检索结果
    bool SpaceIndex::getResult(ResultSet &result, SpaceNode* &snode) {
        if(result.size() == 1) {
            snode = (SpaceNode*) result[0];
            return true;
        }
        return false;
    }

    // 解析检索结果
    bool SpaceIndex::getResult(ResultSet &result, int &spaceID, std::string &PSSID) {
        if(result.size() == 1) {
            struct SpaceNode* snode = (SpaceNode*) result[0];
            spaceID = snode->spaceID;
            PSSID = snode->PSSID;
            return true;
        }
        return false;
    }

   
    // search the object node according to the adcode, if find it will return the object node, else it will
    // return the insertable node
    bool SpaceIndex::search(std::string adcode, SpaceNode* &node) {
        
        SpaceNode* rootNode = (SpaceNode*) (this->_entrance.rootNode);
        if(rootNode->CSNode.size() == 0) {
            node = rootNode;
            return false;
        }

        SpaceNode* backNode = rootNode;
        std::queue<struct SpaceNode*> searchList; 
        for(auto spaceNode : rootNode->CSNode) {
            searchList.push(spaceNode);
        }

        int totalSteLen = adcode.length();
        int subStrLen = 2;             
        while(searchList.empty() == false) {
            node = searchList.front();
            searchList.pop();

            std::string code = adcode.substr(0, subStrLen);
            if(code.compare(node->adCode) == 0) {
                std::queue<struct SpaceNode*> empty;
                std::swap(searchList, empty);
                
                backNode = node;
                if(subStrLen == totalSteLen) {
                    node = backNode;
                    return true;
                }

                std::vector<SpaceNode*>::iterator itBegin = node->CSNode.begin();
                std::vector<SpaceNode*>::iterator itEnd = node->CSNode.end();
                while(itBegin != itEnd) {
                    searchList.push(*itBegin);
                    itBegin++;
                }
                subStrLen += 2;
            } 
        }
        node = backNode;
        return false;

    }

    // search the object node according to the adcode, if find it will return the object node, else it will
    // return the insertable node
    bool SpaceIndex::search(SearchTerm &term, ResultSet &result) {

        SpaceNode* node = nullptr;
        std::string adcode;
        if(this->getTerm(term, adcode) == false) {
            result.push_back(node);
            return false;
        }

        if(this->search(adcode, node) == true) {
            result.push_back(node);
            return true;
        }
        else {
            result.push_back(node);
            return false;
        }
    }


    // insert the node into space index 
    bool SpaceIndex::insert(std::string adcode, SpaceNode* &node) {

        node = new SpaceNode();
        SpaceNode *rootNode = (SpaceNode*) (this->_entrance.rootNode);
       
        // there are just the root node 
        if(rootNode->CSNode.size() == 0) {
        
            node->spaceID = 1;                          
            node->adCode = adcode;
            rootNode->CSNode.push_back(node);
            node->PSNode = rootNode;
            this->_entrance.nodeNum ++;

            std::cout << "创建空间索引根节点,SpaceID为:" << node->spaceID 
            << ",行政编码为:" << node->adCode << std::endl;
            return true;
        }

        SpaceNode *inserted = nullptr; 
        if(this->search(adcode, inserted) == false) {

            node->spaceID = inserted->CSNode.size() + 1;    
            node->adCode = adcode;                 
            inserted->CSNode.push_back(node);
            node->PSNode = inserted;
            this->_entrance.nodeNum ++;

            if(inserted != this->_entrance.rootNode) {
                node->PSSID = inserted->getCompleteSpaceID();  
            }

            std::cout << "创建空间索引根节点,SpaceID为:" << node->spaceID << ",行政编码为:" 
            << node->adCode << ", 其父空间ID为:" << node->PSSID.data() << std::endl; 
            return true;
        } else {
            delete(node);
            node = inserted;
            return false;
        }
        
    }

    // insert the node into space index and the search term is adcode 
    bool SpaceIndex::insert(SearchTerm &term, ResultSet &result)  {
   
        SpaceNode *node = nullptr;
        std::string adcode;

        if(this->getTerm(term, adcode) == false)  {
            result.push_back(node);
            return false;
        }

        // insert mulptile node accoring the relationship of adcode
        int totalSteLen = adcode.length();
        int subStrLen = 2;  
        while(subStrLen <= totalSteLen) {
            std::string code = adcode.substr(0, subStrLen);
            if(this->insert(code, node) == false) {
                if(subStrLen == totalSteLen) {
                    result.push_back(node);
                    return false;
                }
            } 
            subStrLen += 2;
        }

        result.push_back(node);
        return true;

    }

    bool SpaceIndex::remove(SearchTerm &term, ResultSet &result)  {

    }
    
    bool SpaceIndex::update(SearchTerm &oldTerm, SearchTerm &newTerm, ResultSet &result) {

    }     
    
    bool SpaceIndex::persist(std::string fileName) {

    }
    

}