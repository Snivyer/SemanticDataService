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
    };

    // 连续时间段描述符
    struct TSDesc
    {
        tm reportT;             // 起报时间
        tm startT;              // 时间段开始时间
        tm endT;                // 时间段结束时间
        time_t interval;        // 时间间隔
        int count;              // 该时间段中的时间点数量
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
                shapeStr += " ";  
            }
            return shapeStr;
        }
    };


    // 变量列表描述符
    struct VLDesc
    {
        std::unordered_map<std::string, int> varID; // 变量ID列表
        std::vector<VarDesc> desc;        // 变量描述符列表 
        int groupLen;                     // 变量组长度          
    };  

    // 内容结构体
    struct ContentDesc  
    {
        SSDesc ssDesc;       // 语义空间描述符      
        TSDesc tsDesc;       // 时间段描述符
        VLDesc vlDesc;       // 变量列表描述符
    };
 
     // 内容ID描述符
    struct ContentID
    {
        std::string spaceID;            // 空间ID
        std::string timeID;             // 时间ID
        std::string varID;              // 变量ID
        std::vector<size_t> storeIDs;  // 存储空间ID，支持多个存储位置，方便寻址    

        size_t getBestStoID() {
            if(storeIDs.empty()) {
                return 0;
            } else {
                return storeIDs[0];
            }
        }  

        void print() {
            std::cout << "space ID:" << spaceID.data() << std::endl;
            std::cout << "Time ID:" << timeID.data() << std::endl;
            std::cout << "Var ID:" << varID.data() << std::endl;
        }
    };

    


}