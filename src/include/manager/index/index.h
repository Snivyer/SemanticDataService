/* Author: Snivyer
*  Create on: 2023/11/14
*  Description:
   BaseIndex is a base class, which provide the basic interface, such as insert, remove, persist.
*/


#pragma once
#include <string>
#include <vector>

namespace SDS
{
    
    using SearchTerm = std::vector<std::string>;        // 定义索引的检索词为字符串列表
    using ResultSet = std::vector<void *>;              // 定义索引的结果集合为指针类型

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
        void*  rootDesc;        // 根节点描述符
    };



    class BaseIndex
    {
    public:
        IndexEntrance _entrance;      // 索引入口
        virtual bool search(SearchTerm &term, ResultSet &result) = 0;       // 查询节点
        virtual bool insert(SearchTerm &term, 
        
         ResultSet &result) = 0;      // 插入节点
        virtual bool remove(SearchTerm &term, ResultSet &result) = 0;      // 移除节点
        virtual bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result) = 0;               // 更新节点
        virtual bool persist(std::string fileName) = 0; 

    };


}
