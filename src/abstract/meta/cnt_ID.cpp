#include "abstract/meta/cnt_ID.h"

namespace SDS {


    size_t ContentID::getBestStoID() {
        if(storeIDs.empty()) {
            return 0;
        } else {
            return storeIDs[0];
        }
    }  

    ContentID::ContentID(const std::string spaceID, 
                        const std::string timeID,
                        const std::string varID) {
        setSpaceID(spaceID);
        setTimeID(timeID);
        setVarID(varID);
    }


    ContentID::ContentID(std::vector<std::string> identify) {

        if(identify.size() > 0) {
            setSpaceID(identify[0]);
        }
        
        if(identify.size() > 1) {
            setTimeID(identify[1]);
        }

        if(identify.size() > 2) {
            setVarID(identify[2]);
        }

    }

    std::string ContentID::getSpaceID() const  {
        return spaceID;
    }

    void ContentID::setSpaceID(const std::string ID)  {
        this->spaceID = ID;
        
    }

    std::string ContentID::getTimeID() const {
        return timeID;
    }

    void ContentID::setTimeID(const std::string ID) {
        this->timeID = ID;
    }

    std::string ContentID::getVarID() const {
        return varID;
    }

    void ContentID::setVarID(const std::string ID) {
         this->varID = ID;
    }



    bool ContentID::operator==(const ContentID &cntID) const {

        return this->spaceID == cntID.spaceID && this->timeID == cntID.timeID && this->varID == cntID.varID;
    }


    void ContentID::print() {
        std::cout << "space ID:" << spaceID.data() << std::endl;
        std::cout << "Time ID:" << timeID.data() << std::endl;
        std::cout << "Var ID:" << varID.data() << std::endl;
    }



}