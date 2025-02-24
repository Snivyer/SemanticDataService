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

        void printWithTreeModel() {
            std::cout << "  "; 
            std::cout << "  ";
            std::cout << "├─ " << geoName << "(" + adCode + ")" << std::endl;
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

        void printWithTreeModel() {
            std::cout << "  "; 
            std::cout << "  ";
            std::cout << "  ";
            std::cout << "├─ [" << std::put_time(&reportT, "%Y-%m-%d %H:%M:%S") << "] - [" << std::put_time(&endT, "%Y-%m-%d %H:%M:%S")  
                               << "] - [" <<  count  << "]" << std::endl;
        }
    };


    // 变量描述符
    struct VarDesc 
    {
        std::string varName;           
        int varLen;                     
        double resRation;              
        std::string varType;            
        Dimes shape;                    
        int ncVarID;                    
        int ncGroupID;       
        std::string groupPath;        
        std::unordered_map<std::string, std::string> attrs;
        
        void setVarDesc(std::string varName, int varLen, double resRation,
                        std::string varType, Dimes &shape, int ncVarID, int ncGroupID, 
                        std::string groupPath, std::unordered_map<std::string, std::string> &attrs) {
            this->varName = varName;
            this->varLen = varLen;
            this->resRation = resRation;
            this->varType = varType;
            this->shape = shape;
            this->ncGroupID = ncGroupID;
            this->ncVarID = ncVarID;
            this->groupPath = groupPath;

            for(auto item : attrs) {
                this->attrs.insert(item);
            }
        }

        void setVarDesc(VarDesc &desc) {
            varName = desc.varName;
            varLen = desc.varLen;
            resRation = desc.resRation;
            varType = desc.varType;
            shape = desc.shape;
            ncVarID = desc.ncVarID;
            ncGroupID = desc.ncGroupID;
            groupPath = desc.groupPath;
            for(auto item: desc.attrs) {
                attrs.insert(item);
            }
        }

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
        std::unordered_map<std::string, int> varID; 
        int groupLen; 
        std::vector<VarDesc> desc;        
        int attrLen;            
        std::unordered_map<std::string, std::string> attrs;       
        
        void print() {
            std::cout << "变量组内变量数量:" << groupLen << std::endl;
            for(int i = 0; i < groupLen; i++) {
                std::cout << "---------------------------------" << std::endl;
                std::cout << "变量ID:" << i << std::endl;
                std::cout << "变量名:" << desc[i].varName << std::endl;
                std::cout << "变量长度:" << desc[i].varLen << std::endl;
                std::cout << "分辨率大小:" << desc[i].resRation << std::endl;
                std::cout << "变量类型:" << desc[i].varType << std::endl;
                std::cout << "变量维度:" << desc[i].shapeTostr() << std::endl;;
                std::cout << "属性数量:" << desc[i].attrs.size() << std::endl;
                std::cout << "变量于文件内ID:" << desc[i].ncVarID << std::endl;
                std::cout << "变量于文件组ID:" << desc[i].ncGroupID << std::endl;    
                std::cout << "变量于文件组路径" << desc[i].groupPath << std::endl;      
            }

            std::cout << "变量组内属性数量:" << groupLen << std::endl;
            for(auto item: attrs) {
                std::cout << "属性名:" << item.first << "\t\t\t\t\t\t 属性值:" << item.second << std::endl;     
            }
        }

        void printWithTreeModel() {
            for(int i = 0; i < groupLen; i++) {
                std::cout << "  "; 
                std::cout << "  ";
                std::cout << "  ";
                std::cout << "  ";
                std::cout << "├─ " << desc[i].varName << std::endl;
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

        void setTimeSlotDesc(time_t reportT, time_t startT, time_t endT,
                             time_t interval, int count)  {
    
            gmtime_r(&reportT, &(tsDesc.reportT));
            gmtime_r(&startT, &(tsDesc.startT));
            gmtime_r(&endT, &(tsDesc.endT));
            tsDesc.interval = interval;
            tsDesc.count = count;
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

        void setVarListDesc(std::string groupName, int groupLen, 
                                std::unordered_map<std::string, std::string> attrs) {
            vlDesc.groupName = groupName;
            vlDesc.groupLen = groupLen;
            for(auto item: attrs) {
                vlDesc.attrs.insert({item});
            }
        }

        void setVarListVarDesc(std::vector<VarDesc> &varDesc) {
            int i = 0;
            for(auto item: varDesc) {
                VarDesc desc;
                desc.setVarDesc(item);
                vlDesc.desc.push_back(desc);
                vlDesc.varID.insert({desc.varName, i});
                i++;
            }
        }

        void print() {
            ssDesc.print();
            tsDesc.print();
            vlDesc.print();       
        }

        void printWithTreeModel() {
            ssDesc.printWithTreeModel();
            tsDesc.printWithTreeModel();
            vlDesc.printWithTreeModel();
        }


    };
 
  
}