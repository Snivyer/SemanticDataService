#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "abstract/utils/string_operation.h"
#include "arrow/util/logging.h"


namespace SDS {

       // 内容ID描述符
    class ContentID {
    public: 

        std::vector<size_t> storeIDs;  // 存储空间ID，支持多个存储位置，方便寻址
        ContentID(const std::string spaceID ="", const std::string TimeID ="", const std::string varID ="");
        ContentID(std::vector<std::string> identify);

        size_t getBestStoID();
        bool operator==(const ContentID &cntID) const;
        void print();
    
        std::string getSpaceID() const;
        void setSpaceID(const std::string ID);
        std::string getTimeID() const;
        void setTimeID(const std::string ID);
        std::string getVarID() const;
        void setVarID(const std::string ID);


    private:
        std::string spaceID;            // 空间ID
        std::string timeID;             // 时间ID
        std::string varID;              // 变量ID
    };

    struct ContentIDHasher {
        std::size_t operator()(const ContentID &cntID) const {

            std::size_t seed = 0;
            cntHash(seed, cntID.getSpaceID());
            cntHash(seed, cntID.getTimeID());
            cntHash(seed, cntID.getVarID());
            return seed;
        }

        void cntHash(std::size_t &seed, const std::string id) const {
            std::hash<std::string> hasher;
            seed ^= hasher(id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

}