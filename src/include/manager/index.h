/* Author: Snivyer
*  Create on: 2023/11/14
*  Description:
   BaseIndex is a base class, which provide the basic interface, such as insert, remove, persist.
*/


#ifndef INDEX_H_
#define INDEX_H_
#include <string>
#include <vector>

namespace SDS
{
    
    using SearchTerm = std::vector<std::string>;        // 定义索引的检索词为字符串列表
    using ResultSet = std::vector<void *>;                 // 定义索引的结果集合为数值列表

    // 索引构建的类型
    enum IndexType
    {
        Space,
        Time,
        TimeSlot,
        Var,
        VarList,
    };

    // 索引根节点
    struct IndexEntrance
    {
        IndexType type;         // 索引类型
        void*  rootNode;        // 索引根节点
        size_t nodeNum;         // 检索数量
    };

    class BaseIndex
    {
    public:
        struct IndexEntrance entrance;      // 索引入口

        BaseIndex(struct IndexEntrance &entrance) 
        {
            this->entrance = entrance;
        }

        ~BaseIndex();
        
        virtual bool search(SearchTerm &term, ResultSet &result);       // 查询节点
        virtual bool insert(SearchTerm &term, ResultSet &result);       // 插入节点
        virtual bool remove(SearchTerm &term, ResultSet &result);       // 移除节点
        virtual bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result);               // 更新节点
        virtual bool persist(std::string fileName); 

    };


}



#endif