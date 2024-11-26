#include "manager/index/time_index.h"


namespace SDS {


    TimeIndex::TimeIndex() {

        _entrance.type = IndexType::TimeSlot;
        // todo: 需要从缓存中读取
        // Timeslot Index is loaded in terms of a fixed time period

    }

    TimeIndex::~TimeIndex() {

        // todo: 持久化到缓存中
        
    }

    bool TimeIndex::getTerm(SearchTerm &term, time_t &reportTime) {
       
        if(term.size() == 1) {
            string_to_time(term[0], reportTime);
            return true;
        }

        return false;
    }

    bool TimeIndex::getResult(ResultSet &result, TimeSlotNode* &snode) {
        if(result.size() == 1) {
            snode = (TimeSlotNode*) result[0];
            return true;
        }
        return false;
    }
        
    bool TimeIndex::search(SearchTerm &term, ResultSet &result) {

        TimeSlotNode* resultNode = nullptr;
        result.push_back(resultNode);
        time_t reportTime;

        if(this->getTerm(term, reportTime) == false) {
            return false;
        }

        return search(reportTime, resultNode);
    }

    bool TimeIndex::search(time_t reportTime, TimeSlotNode* &node) {
       
        auto ret = timeSlotIndex.find(reportTime);
        if(ret == timeSlotIndex.end()) {
            return false;
        }

        node = *(ret->second);
        return true;
    }

    bool TimeIndex::insert(SearchTerm &term, ResultSet &result) {
        
        TimeSlotNode* resultNode = nullptr;
        result.push_back(resultNode);
        time_t reportTime;

        if(this->getTerm(term, reportTime) == false) {
            return false;
        }

        return insert(reportTime, resultNode);
    }

    bool TimeIndex::insert(time_t reportTime, TimeSlotNode* &node) {
        
        auto ret = timeSlotIndex.find(reportTime);
        if(ret != timeSlotIndex.end()) {
            return false;  
        }

        node = new TimeSlotNode;
        // firstly, insert into timeSlotSet
        timeSlotSet.push_back(node);
        node->timeSlotID = timeSlotSet.size();

        // secondly, insert into timeSlotIndex
        timeSlotIndex.insert({reportTime,  timeSlotSet.end()});
        timeSlotIndexWithID.insert({node->timeSlotID,  timeSlotSet.end()});
        return true;
    }
    
    bool TimeIndex::remove(SearchTerm &term, ResultSet &result) {

    }

    bool TimeIndex::update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result) {

                            
    }

    bool TimeIndex::persist(std::string fileName) {

    }





}