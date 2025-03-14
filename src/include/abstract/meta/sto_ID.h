#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "abstract/utils/string_operation.h"
#include "arrow/util/logging.h"


namespace SDS {

    class StorageID {
    public: 
    
        StorageID(const std::string spaceID = "", const std::string typeID ="", const std::string siteID ="");
        StorageID(std::vector<std::string> identify);

        bool operator==(const StorageID &stoID) const;
        void print() const;
        void printWithTreeModel() const;
    
        std::string getSpaceID() const;
        void setSpaceID(const std::string ID);
        std::string getTypeID() const;
        void setTypeID(const std::string ID);

        std::string getSiteID() const;
        void setSiteID(const std::string ID);


    private:
        std::string spaceID;             // space ID 
        std::string typeID;             // type ID
        std::string siteID;            // site ID
    };

    struct StorageIDHasher {
        std::size_t operator()(const StorageID &stoID) const {
            std::size_t seed = 0;
            stoHash(seed, stoID.getSpaceID());
            stoHash(seed, stoID.getTypeID());
            stoHash(seed, stoID.getSiteID());
            return seed;
        }

        void stoHash(std::size_t &seed, const std::string id) const {
            std::hash<std::string> hasher;
            seed ^= hasher(id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

}