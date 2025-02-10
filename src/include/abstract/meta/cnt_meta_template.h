/* Author: Snivyer
*  Create on: 2024/3/26
*  Description:
    define various content metadata template
*/

#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "abstract/utils/string_operation.h"
#include "abstract/utils/time_operation.h"

namespace SDS {

    using Dimes = std::vector<size_t>;          // 变量维度
    
    // 地理坐标结构体
    struct GeoCoordinate
    {
        double logitude;         // 经度
        double latitude;         // 纬度
    };

    // 空间描述符
    struct SSDesc
    {
        std::string geoName;                        // 地理空间名
        std::string adCode;                         // 地理行政编码
        GeoCoordinate geoCentral;                   // 中心经纬度
        std::vector<GeoCoordinate> geoPerimeter;    // 地理周界

        void print() {
            std::cout << "地理空间名字： " << geoName << std::endl;
            std::cout << "行政编码： " << adCode << std::endl;
            std::cout << "中心经纬度： " <<  geoCentral.logitude <<" , "
                        << geoCentral.latitude << std::endl;
    
            std::cout << "地理周界: "<< std::endl;
            for(int i = 0; i < geoPerimeter.size(); i++){
                std::cout << "      " << geoPerimeter[i].logitude
                        <<" , "<< geoPerimeter[i].latitude << std::endl; 
            }
        }
    };

    // 连续时间段描述符
    struct TSDesc
    {
        tm reportT;
        tm startT;              // 时间段开始时间
        tm endT;                // 时间段结束时间
        time_t interval;        // 时间间隔
        int count;              // 该时间段中的时间点数量

        void print() {
            if(count > 0) {
                std::cout << "起报时间:" << std::put_time(&reportT, "%Y-%m-%d %H:%M:%S") << std::endl;
                std::cout << "时间段开始时间:" << std::put_time(&startT, "%Y-%m-%d %H:%M:%S") << std::endl;
                std::cout << "时间段结束时间:" << std::put_time(&endT, "%Y-%m-%d %H:%M:%S") << std::endl;
                std::cout << "时间段间隔(秒):" << interval << std::endl;
                std::cout << "时间段中时间点数量:" << count << std::endl;
            } else {
                std::cout << "起报时间:" << std::put_time(&reportT, "%Y-%m-%d %H:%M:%S") << std::endl;
                std::cout << "时间段中时间点数量:" << count << std::endl;
            }
        }

    };


    // 变量描述符
    struct VarDesc 
    {
        std::string varName;            // 变量名
        int varLen;                     // 变量长度
        double resRation;               // 分辨率大小
        std::string varType;            // 变量类型
        Dimes shape;                    // 变量大小
        
        int ncVarID;                    // 变量对应的nc文件ID
        int ncGroupID;                  // 变量对应的nc文件组ID

        std::string shapeTostr() {
            std::string shapeStr;
            for(int i = 0; i < shape.size(); i++) {
                shapeStr += std::to_string(shape[i]);
                shapeStr += "-";  
            }
            return shapeStr;
        }

        void strToShape(std::string shapeStr) {
            std::vector<std::string> shapeStrList = splitString(shapeStr, '-');

            for(auto shapeStr : shapeStrList) {
                shape.push_back(std::stoi(shapeStr));
            }
        }


    };


    // 变量列表描述符
    struct VLDesc {
        std::string groupName;
        std::unordered_map<std::string, int> varID; // 变量ID列表
        std::vector<VarDesc> desc;        // 变量描述符列表 
        int groupLen;                     // 变量组长度    
        
        void print() {
            std::cout << "变量组内变量数量:" << groupLen << std::endl;

            for(int i = 0; i < groupLen; i++) {
                std::cout << "变量ID:" << i << std::endl;
                std::cout << "变量名:" << desc[i].varName << std::endl;
                std::cout << "变量长度:" << desc[i].varLen << std::endl;
                std::cout << "分辨率大小:" << desc[i].resRation << std::endl;
                std::cout << "变量类型:" << desc[i].varType << std::endl;
                std::cout << "变量维度:" << desc[i].shapeTostr() << std::endl;;
                std::cout << "NC文件ID:" << desc[i].ncVarID << std::endl;
                std::cout << "NC文件组ID:" << desc[i].ncGroupID << std::endl;
                std::cout << "---------------------------------" << desc[i].ncGroupID << std::endl;
            }
        }
    };  

    // 内容结构体
    struct ContentDesc {
        SSDesc ssDesc;       // 语义空间描述符      
        TSDesc tsDesc;       // 时间段描述符
        VLDesc vlDesc;       // 变量列表描述符

        void setSpaceDesc(std::string geoName, std::string adCode,
                            GeoCoordinate &geoCentral, std::vector<GeoCoordinate>  &geoPerimeter) {
            ssDesc.geoName = geoName;
            ssDesc.adCode = adCode;
            ssDesc.geoCentral.latitude = geoCentral.latitude;
            ssDesc.geoCentral.logitude = geoCentral.logitude;

            for(auto item : geoPerimeter) {
                GeoCoordinate geo;
                geo.latitude = item.latitude;
                geo.logitude = item.logitude;
                ssDesc.geoPerimeter.push_back(geo);
            }
        }

        void setSpaceDesc(std::string geoName, std::string adCode, double logitude, double latitude,
                            std::vector<GeoCoordinate> &geoCoor) {
       
            ssDesc.geoName = geoName;
            ssDesc.adCode = adCode;
            ssDesc.geoCentral.latitude = latitude;
            ssDesc.geoCentral.logitude = logitude;

            for(auto item : geoCoor) {
                GeoCoordinate geo;
                geo.latitude = item.latitude;
                geo.logitude = item.logitude;
                ssDesc.geoPerimeter.push_back(geo);
            }
        }

        void setSpaceDesc(SSDesc &desc) {
            ssDesc.geoName = desc.geoName;
            ssDesc.adCode = desc.adCode;
            ssDesc.geoCentral.latitude = desc.geoCentral.latitude;
            ssDesc.geoCentral.logitude = desc.geoCentral.logitude;


            for(auto item : desc.geoPerimeter) {
                GeoCoordinate geo;
                geo.latitude = item.latitude;
                geo.logitude = item.logitude;
                ssDesc.geoPerimeter.push_back(geo);
            }
        }

        void setTimeSlotDesc(TSDesc &desc)  {
            tsDesc.reportT = desc.reportT;
            tsDesc.startT = desc.startT;
            tsDesc.endT = desc.endT;
            tsDesc.interval = desc.interval;
            tsDesc.count = desc.count;
        }

        void setVarListDesc(VLDesc &desc) { 
            vlDesc.groupName = desc.groupName;
            vlDesc.groupLen = desc.groupLen;
            for(auto item : desc.varID) {
                vlDesc.varID.insert({item.first, item.second});
            }
        }

        void print() {
            ssDesc.print();
            tsDesc.print();
            vlDesc.print();       
        }


    };
 
  
}