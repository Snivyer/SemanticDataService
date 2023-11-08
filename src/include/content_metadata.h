/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   ContentMetadata is child class of Metadata, which is reponsible for management of the
   content metadata and construction of content index.
*/

#ifndef CONTENT_METADATA_H_
#define CONTENT_METADATA_H_


#include "metadata.h"
#include <string>
#include <vector>


struct GeoCoordinate
{
    float logitude;
    float latitude;
};

// 空间描述符
struct SSDesc
{
    
    std::string geoName;                        // 地理空间名
    std::string adCode;                         // 地理行政编码
    vector<GeoCoordinate> geoCentral;           // 中心经纬度
    vector<Vecor<GeoCoordinate>> geoPerimeter;  // 地理周界
};

struct ContentDesc
{
    struct SSDesc;
};

namespace SDS
{
    class ContentMetadata
    {
    public:
        ContentDesc* desc;

    
        void extractSSDesc(std::string geoName);

        

    }


}


#endif