#include "manager/index/site_index.h"
#include <iostream>

namespace SDS {

    // 初始化空间索引
    SiteIndex::SiteIndex() {
        SiteNode *node = new SiteNode();
        node->siteID = 1;
        node->siteName = "root";
        _entrance.type = IndexType::Site;
        _entrance.rootNode = node;
        _entrance.nodeNum = 0;
    }

    // 析构函数
    SiteIndex::~SiteIndex() {
        if(this->_entrance.rootNode != NULL) {
            SiteNode* node = (SiteNode*) _entrance.rootNode;

            // 构建解析队列
            std::queue<struct SiteNode*> destructorList;       
            destructorList.push(node);
            

            while(destructorList.empty() == false) {
                node = destructorList.front();
                destructorList.pop();

                std::vector<SiteNode*>::iterator itBegin = node->CSNode.begin();
                std::vector<SiteNode*>::iterator itEnd = node->CSNode.end();

                // 依次将子节点加入到析构队列中，完成空间释放
                while(itBegin != itEnd) {
                    destructorList.push(*itBegin);
                    itBegin++;
                }
                delete node;
                  
            }
        }  
    }

    // 解析检索词
    bool SiteIndex::getTerm(SearchTerm &term, std::string &path) {
        if(term.size() == 1) {
            path = term[0];
            return true;
        }

        return false;
    }

    // 解析检索结果
    bool SiteIndex::getResult(ResultSet &result, SiteNode* &snode) {
        if(result.size() == 1) {
            snode = (SiteNode*) result[0];
            return true;
        }
        return false;
    }

    bool SiteIndex::search(std::string siteName, SiteNode* &node, std::vector<SiteNode*> &searchList) {
        if(searchList.size() == 0) {
            return false;
        }

        for(auto item : searchList) {
            if(item->siteName == siteName) {
                node = item;
                return true;
            }
        }
        return false;
    }

    // search the object node according to site path, if find it will return the object node, else it will
    // return the insertable node
    bool SiteIndex::search(std::string path, SiteNode* &node) {
        node = (SiteNode*) (this->_entrance.rootNode);
        std::vector<std::string> siteNames = splitString(path, '/');
        for(int i = 0; i < siteNames.size(); i++) {
            if(search(siteNames[i], node, node->CSNode) == false) {
                return false;
            }
        }
        return true;
    }

    // search the object node according to the adcode, if find it will return the object node, else it will
    // return the insertable node
    bool SiteIndex::search(SearchTerm &term, ResultSet &result) {
        SiteNode* node = nullptr;
        result.push_back(node);
        std::string path;
        if(this->getTerm(term, path) == false) {
            return false;
        }
        return search(path, node);
    }


    bool SiteIndex::insert(std::string siteName, SiteNode* &node, SiteNode* pNode) {
        if(pNode == nullptr) {
            return false;
        }

        if(siteName == pNode->siteName) {
            node = pNode;
            return true;
        } 

        if(pNode->CSNode.size() == 0) {
            node = new SiteNode();
            node->siteID = 1;                          
            node->siteName = siteName;
            node->PSiteID = pNode->getCompleteSiteID();
            node->PSiteNode = pNode;
            pNode->CSNode.push_back(node);
            std::cout << "创建站点索引根节点,SiteID为:" << node->siteID
                      << ",站点名为:" << node->siteName.data() << ", 其父站点ID为:" 
                      << node->PSiteID.data() << std::endl;   
            return true;
        } 
                
        auto ret = search(siteName, node, pNode->CSNode);
        if(!ret) {
            node = new SiteNode();
            node->siteID = pNode->CSNode.size() + 1;
            node->siteName = siteName;      
            node->PSiteID = pNode->getCompleteSiteID();
            node->PSiteNode = pNode;
            pNode->CSNode.push_back(node);
            std::cout << "创建空间索引根节点,SiteID为:" << node->siteID << ",站点名为:" 
                << node->siteName.data() << ", 其父站点ID为:" << node->PSiteID.data() << std::endl; 
        }
        return true;
    }


    // insert the node into site index and the search term is path
    bool SiteIndex::insert(SearchTerm &term, ResultSet &result)  {
        SiteNode *node = nullptr;
        std::string path;
        if(this->getTerm(term, path) == false)  {
            result.push_back(node);
            return false; 
        }

        // insert mulptile node accoring the relationship of adcode
        std::vector<std::string> siteNames = splitString(path, '/');
        SiteNode *pNode = (SiteNode*) (this->_entrance.rootNode);

        for(int i = 0; i < siteNames.size(); i++) {
            if(insert(siteNames[i], node, pNode)) {
                pNode = node;
            } else {
                result.push_back(node);
                return false;
            }
        }

        result.push_back(node);
        return true;

    }

    bool SiteIndex::remove(SearchTerm &term, ResultSet &result)  {

    }
    
    bool SiteIndex::update(SearchTerm &oldTerm, SearchTerm &newTerm, ResultSet &result) {

    }     
    
    bool SiteIndex::persist(std::string fileName) {

    }
    

}