/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   data management class is to implement the basic operations of semantic data service
*/

#ifndef DATA_MANAGEMENT_H_
#define DATA_MANAGEMENT_H_
#include <string>
#include <vector>
#include <map>
#include "manager/cnt_meta.h"

namespace SDS
{

    class dataManager
    {
    public:
        bool LoadSS(std::string spaceID);
        bool LoadSS(std::string geoName);


        bool importData(std::string spaceID, std::vector<std::string> filePathList, bool justMeta);


    private:
        std::map<std::string, std::string> geoName2SID;
        std::map<std::string, ContentMeta> SID2CntMeta;
        



    };



}
   





#endif