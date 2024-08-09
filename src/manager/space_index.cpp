#include "manager/space_index.h"
#include <iostream>

namespace SDS {

    // 初始化空间索引
    SpaceIndex::SpaceIndex(struct IndexEntrance &entrance):BaseIndex(entrance)
    {
        // 声明索引类型为空间索引
        this->entrance.type = IndexType::Space;
        this->entrance.rootNode = NULL;
        this->entrance.nodeNum = 0;
    }

    // 析构函数
    SpaceIndex::~SpaceIndex()
    {
    
        if(this->entrance.rootNode != NULL)
        {
            struct SpaceNode* node = (SpaceNode*) this->entrance.rootNode;

            // 构建解析队列
            std::queue<struct SpaceNode*> destructorList;       
            destructorList.push(node);
            

            while(destructorList.empty() == false)
            {
                node = destructorList.front();
                destructorList.pop();

                std::vector<SpaceNode*>::iterator itBegin = node->CSNode.begin();
                std::vector<SpaceNode*>::iterator itEnd = node->CSNode.end();

                // 依次将子节点加入到析构队列中，完成空间释放
                while(itBegin != itEnd)
                {
                    destructorList.push(*itBegin);
                    itBegin++;
                }

                std::cout << "析构行政编码为:" << node->me->desc.adCode  << "节点！" << std::endl; 
                delete node;
                  
            }
        }  
    }

    // 解析检索词
    bool SpaceIndex::getTerm(SearchTerm &term, std::string &adcode)
    {
       
        if(term.size() == 1)
        {
            // 使用行政编码作为检索词
            adcode = term[0];
            return true;
        }

        return false;
    }

    // 解析检索结果
    bool SpaceIndex::getResult(ResultSet &result, struct SpaceNode* snode)
    {
        if(result.size() == 1)
        {
            snode = (SpaceNode*) result[0];
            return true;
        }
        return false;
    }

    // 解析检索结果
    bool SpaceIndex::getResult(ResultSet &result, int &spaceID, std::string &PSSID)
    {
        if(result.size() == 1)
        {
            struct SpaceNode* snode = (SpaceNode*) result[0];
            spaceID = snode->SpaceID;
            PSSID = snode->PSSID;
            return true;
        }
        return false;
    }




    /**
     * 根据空间的行政编码，检索对应的空间索引节点
     * @param 行政编码
     * @param 检索成功返回成功节点，检索失败返回可插入节点
    */
    bool SpaceIndex::search(std::string adcode, struct SpaceNode* &node)
    {
        node = (SpaceNode*) this->entrance.rootNode;

        if(this->entrance.rootNode == NULL)
        {
            // 此时根节点作为插入点
            return false;
        }

        // 构建检索队列
        std::queue<struct SpaceNode*> searchList;       
        searchList.push(node);
        
        int totalSteLen = adcode.length();
        int subStrLen = 2;              

        while(searchList.empty() == false)
        {

            node = searchList.front();
            searchList.pop();
            std::string code = adcode.substr(0, subStrLen);

            if(code.compare(node->me->desc.adCode) == true)
            {
            
                // 找到了，意味着后续的不需要找，直接清空列表
                std::queue<struct SpaceNode*> empty;
                std::swap(searchList, empty);

                // 如果全部找完了，且长度相同
                if(subStrLen == totalSteLen)
                {
                    return true;
                }

                // 否则开始下一轮的匹配
                std::vector<SpaceNode*>::iterator itBegin = node->CSNode.begin();
                std::vector<SpaceNode*>::iterator itEnd = node->CSNode.end();

                // 依次将子节点加入到检索队列中
                while(itBegin != itEnd)
                {
                    searchList.push(*itBegin);
                    itBegin++;
                }
                subStrLen += 2;
            } 
        }

        return false;

    }

    bool SpaceIndex::search(SearchTerm &term, ResultSet &result)
    {
        // 检索结果
        struct SpaceNode* node = NULL;

        // 空间索引检索采用行政编码作为检索词
        std::string adcode;
        if(this->getTerm(term, adcode) == false)
        {
            result.push_back(node);
            return false;
        }

        if(this->search(adcode, node) == true)
        {
            result.push_back(node);
            return true;
        }
        else
        {
            result.push_back(node);
            return false;
        }
    }



bool SpaceIndex::insert(std::string adcode, SpaceDescList* &me, std::string PSSID) {

    SpaceNode *node = new SpaceNode;
    node->me = me;
    node->PSSID = PSSID;
    node->parent = nullptr;
    node->PSNode = nullptr;
    node->children = nullptr;
    node->CSNode.clear();

    return insert(adcode, node);
}

    /**
     * 在空间索引中插入行政编码相对应的节点
     * @param 行政编码
     * @param 返回插入成功的空间索引节点
    */
    bool SpaceIndex::insert(std::string adcode, struct SpaceNode *node)
    {
      
        // 空间索引为空， 直接插入
        if(this->entrance.rootNode == NULL)
        {
            // step1: 初始化节点
            node->SpaceID = 1;                              // 初始化空间ID
            node->me->SpaceID = node->SpaceID; 

            // step2: 连接索引节点指针
            this->entrance.rootNode = node;
            return true;
        }

        // 待插入节点
        struct SpaceNode *inserted = NULL; 

        // 检索对应的行政编码的节点是否存在，不存在的话，会获得插入点
        if(this->search(adcode, inserted) == false)
        {
            // step1: 初始化节点
            node->me->desc.adCode = adcode;                 // 初始化行政编码
            node->SpaceID = inserted->CSNode.size() + 1;    // 初始化空间ID
            node->PSSID = inserted->SpaceID;                // 初始化父空间ID
          
            // step2: 连接索引节点指针
            inserted->CSNode.push_back(node);
            node->PSNode = inserted;
            node->CSNode.clear();

            // step3: 填充内容描述符

            std::cout << "插入行政编码为：" << node->me->desc.adCode << "的节点，其空间ID为："
            << node->PSSID << ", 其父空间ID为:" << node->PSSID << std::endl; 
        }
        else
        {
            delete(node);
            node = NULL;
            return false;
        }
        
    }

    bool SpaceIndex::insert(SearchTerm &term, ResultSet &result)
    {
        // 创建要插入的索引节点
        struct SpaceNode *node = NULL;

        // 获取行政编码
        std::string adcode;
        if(this->getTerm(term, adcode) == false)
        {
            result.push_back(node);
            return false;
        }

        // 这里需要循环创建节点，如360802会创建36节点，3608节点，360802节点
        int totalSteLen = adcode.length();
        int subStrLen = 2;  

        while(subStrLen < totalSteLen)
        {
            std::string code = adcode.substr(0, subStrLen);

            if(this->insert(code, node) == true)
            {
                subStrLen += 2;
            }
            else
            {
                result.push_back(node);
                return false;          
            }
        }

        result.push_back(node);
        return true;
    }

    bool SpaceIndex::remove(SearchTerm &term, ResultSet &result)
    {

    }
    
    bool SpaceIndex::update(SearchTerm &oldTerm, SearchTerm &newTerm, ResultSet &result)
    {

    }     
    
    bool SpaceIndex::persist(std::string fileName)
    {

    }
    

}