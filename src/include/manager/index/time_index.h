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
#include "manager/index/index.h"
#include "manager/metadata/cnt_meta.h"
#include "abstract/utils/time_operation.h"


namespace SDS
{

    struct TimeList
    {
        size_t timeIntervalID;
        time_t endTime;                 // 结束时间
        std::vector<time_t> timeIndex;
        std::unordered_map<time_t, size_t> timeIDs;

        size_t getTimeID(time_t t) {
            auto ret = timeIDs.find(t);
            if(ret != timeIDs.end()) {
                return ret->second;
            }
            return 0;
        }

        size_t getTimeIntervalID() {
            return timeIntervalID;
        }

        std::string getHemiTimeID(time_t t) {
            int timeID = getTimeID(t);
            if(timeID == 0) {
                return std::to_string(timeIntervalID);
            } else {
                return std::to_string(timeIntervalID) + std::to_string(timeID);
            }
        }

        void insertTimestamp(time_t t) {

            auto ret = timeIDs.find(t);
            if(ret == timeIDs.end()) {
                timeIndex.push_back(t);
                size_t timeID = timeIndex.size();
                timeIDs.insert({t, timeID});
            }
        }

    };

    // 时间段节点
    struct TimeSlotNode
    {
        size_t timeSlotID;                      // 时间段ID
        time_t reportTime;                 // 起报时间
        size_t intervalNums;                    // 时间间隔数量

        std::unordered_map<time_t, TimeList*>  timeIntervalIndex;  // 不同时间分辨率

        TimeSlotNode() {
            intervalNums = 0;
        }

        std::string getTimeSlotID() {
            return std::to_string(timeSlotID);
        }

        std::string getCompleteTimeID(long interval, time_t &t) {

            if(intervalNums == 0) {
                return getTimeSlotID();
            }

            auto ret = timeIntervalIndex.find(interval);
            if(ret != timeIntervalIndex.end()) {
                return getTimeSlotID() + ret->second->getHemiTimeID(t);
            }
            return getTimeSlotID();
        }

        void insertTimeList(time_t interval, TimeList* timeSlot) {
            
            auto ret = timeIntervalIndex.find(interval);
            if(ret == timeIntervalIndex.end()) {
                timeIntervalIndex.insert({interval, timeSlot});
                intervalNums += 1;
            }
        }
    
    };


    class TimeIndex : public BaseIndex
    {
    public:
        TimeIndex();
        ~TimeIndex();
        
        bool search(SearchTerm &term, ResultSet &result);       // 查询节点
        bool insert(SearchTerm &term, ResultSet &result);       // 插入节点
        bool remove(SearchTerm &term, ResultSet &result);       // 移除节点
        bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result);       // 更新节点
        bool persist(std::string fileName); 
        bool search(time_t reportTime,  TimeSlotNode* &node);
        bool insert(time_t reportTime,  TimeSlotNode* &node);

    private:
        std::vector<TimeSlotNode*>   timeSlotSet;
        std::unordered_map<time_t, std::vector<TimeSlotNode*>::iterator> timeSlotIndex;
        std::unordered_map<size_t, std::vector<TimeSlotNode*>::iterator> timeSlotIndexWithID;
        bool getTerm(SearchTerm &term, time_t &reportTime);
        bool getResult(ResultSet &result, TimeSlotNode* &snode);
    };
}
