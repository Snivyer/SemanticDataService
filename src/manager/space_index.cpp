#include "manager/space_index.h"

namespace SDS
{

    // 初始化空间索引
    SpaceIndex::SpaceIndex(struct IndexEntrance &entrance):BaseIndex(entrance)
    {
        // 声明索引类型为空间索引
        this->entrance.type = IndexType::Space;
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

    bool SpaceIndex::search(SearchTerm &term, ResultSet &result)
    {
        
        if(this->entrance.rootNode == NULL)
        {
            return false;
        }

        struct SpaceNode* node = (SpaceNode*) this->entrance.rootNode;
       
        // 构建检索队列
        std::queue<struct SpaceNode*> searchList;       
        searchList.push(node);
        
        std::string adcode;
        if(this->getTerm(term, adcode) == false)
        {
            return false;
        }

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
                    result.push_back(node);
                    return true;
                }

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

        result.push_back(node);
        return false;

    }

    // 这里还有大问题，节点没有传参数进来，哈哈哈哈哈
    bool SpaceIndex::insert(SearchTerm &term, ResultSet &result)
    {

        struct SpaceNode *nnode = new SpaceNode;
        
        if(this->entrance.rootNode == NULL)
        {
            this->entrance.rootNode = nnode;
        }


        ResultSet searchSet;
        if(this->search(term, searchSet) == false)
        {
            struct SpaceNode *node;

            if(this->getResult(searchSet, node) == true)
            {

                node->CSNode.push_back(nnode);
                nnode->PSNode = node;
                return true;
            }

        }
        
        return false;


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