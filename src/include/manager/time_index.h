/* Author: Snivyer
*  Create on: 2023/11/20
*  Description:
   Time Index is a children class of baseIndex, which implement the time index with multiple-level
   list.
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

    struct TimeDescList
    {
        int TimeSlotID;
        struct TSDesc desc;

    };

    // 时间段节点
    struct TimeSlotNode
    {
        size_t TimeSlotID;                      // 时间段ID
        TimeDescList* me;                      // 时间段
        TimeSlotNode* next;                   //  后一个节点

        std::vector<TimeDescList>* children;  // 时间描述符
        TimeSlotNode* CTSNode;               // 子孩子节点
    
    };


    class TimeIndex : BaseIndex
    {
    public:
        TimeIndex(struct IndexEntrance &entrance);
        ~TimeIndex();

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
