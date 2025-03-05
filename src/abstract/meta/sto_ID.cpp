#include "abstract/meta/sto_ID.h"

namespace SDS {

    StorageID::StorageID(const std::string spaceID, 
                        const std::string typeID,
                        const std::string siteID) {
        setSpaceID(spaceID);
        setTypeID(typeID);
        setSiteID(siteID);
    }


    StorageID::StorageID(std::vector<std::string> identify) {
        if(identify.size() > 0) {
            setSpaceID(identify[0]);
        }
        
        if(identify.size() > 1) {
            setTypeID(identify[1]);
        }

        if(identify.size() > 2) {
            setSiteID(identify[2]);
        }
    }

    std::string StorageID::getSpaceID() const  {
        return spaceID;
    }

    void StorageID::setSpaceID(const std::string ID)  {
        this->spaceID = ID;
        
    }

    std::string StorageID::getTypeID() const {
        return typeID;
    }

    void StorageID::setTypeID(const std::string ID) {
        this->typeID = ID;
    }

    std::string StorageID::getSiteID() const {
        return siteID;
    }

    void StorageID::setSiteID(const std::string ID) {
         this->siteID = ID;
    }


    bool StorageID::operator==(const StorageID &stoID) const {
        return this->spaceID == stoID.spaceID && this->typeID == stoID.typeID && this->siteID == stoID.siteID;
    }


    void StorageID::print() const {
        std::cout << "空间ID:" << spaceID.data() << std::endl;
        std::cout << "类型ID:" << typeID.data()  << std::endl;
        std::cout << "站点ID:" << siteID.data()  << std::endl;
    }

    void StorageID::printWithTreeModel() const {
        std::cout << "  ";
        std::cout << "├─ " << spaceID.data() << "-" << typeID.data()  << "-" << siteID.data()  << std::endl;
    }
}