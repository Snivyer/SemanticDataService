/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   ContentMetadata is a children class of Metadata class, which is reponsible for management of the
   content metadata and construction of content index.
*/

#pragma once


#include "meta.h"
#include <string>
#include <vector>
#include <sys/time.h>
#include <string.h>

namespace SDS
{

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
    };


    // 变量列表描述符
    struct VLDesc
    {
        std::string  varListName;            // 变量组名
        std::vector<int> varID;             // 变量ID列表
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
    };


    // 内容元数据管理类
    class ContentMeta: public Metadata
    {
    public:
        ContentMeta();
        ~ContentMeta();

        /*------提取元数据------*/
        // 根据省、市、县提取空间描述符
        void extractSSDesc(ContentDesc &cntDesc, std::string province, std::string city = "0", std::string district = "0");   
        
        // extract the space desciption according to arbitrary number of geoNames
        void extractSSDesc(ContentDesc &cntDesc, std::vector<std::string> &geoNames);
        
        
        void putSSDesc(ContentDesc &cntDesc, MetaStore* &ms);


        void extractTSDesc(ContentDesc &cntDesc, std::vector<std::string> filePathList);

        void extractTSDesc(ContentDesc &cntDesc, std::string startTime, std::string endTime,  // 根据文件名信息，提取时间段描述符
                        time_t inteval, std::string reportTimeString);

        void extractTSDesc(ContentDesc &cntDesc, std::string dirpath);  // 根据目录路径信息，提取时间段描述符;

        void extractVLDesc(ContentDesc &cntDesc, std::string filePath);      // 从NC文件中提取变量列表

        /*------获取元数据------*/
        ContentDesc getCntMeta(ContentID &cntID);


        void printSSDesc(ContentDesc &cntDesc);

        
        void putMetaWithJson(std::string path, MetaStore* &ms) override;
        void putMetaWithCSV(std::string path, MetaStore* &ms) override;
        void putMetaWithParquet(std::string path, MetaStore* &ms) override;   


    private:

    
        bool string_to_tm(std::string timeStr, struct tm &time);       
        bool string_to_time(std::string timeStr, time_t &time);  

        std::string getMeta(Key key) override;
        void putMeta(Key key, std::string value) override;   


    };


}

