#include "manager/cnt_meta.h"


namespace SDS
{

    ContentMeta::ContentMeta()
    {
        this->desc = new ContentDesc();
    }

    ContentMeta::~ContentMeta()
    {
        if(this->desc != NULL)
        {
            delete this->desc;
        }
    }

    // 提取空间描述符
    void ContentMeta::extractSSDesc(std::string geoName)
    {
        // 这里的代码，学弟会给我的

        this->desc->ssDesc.geoName = std::string("江西省赣州市");
        this->desc->ssDesc.adCode = std::string("360732");
        this->desc->ssDesc.geoCentral = {114, 25};
        this->desc->ssDesc.geoPerimeter.push_back(GeoCoordinate{100.0, 110.0});
        this->desc->ssDesc.geoPerimeter.push_back(GeoCoordinate{100.0, 120.0});
        this->desc->ssDesc.geoPerimeter.push_back(GeoCoordinate{130.0, 110.0});
        this->desc->ssDesc.geoPerimeter.push_back(GeoCoordinate{130.0, 120.0});
    }


    bool ContentMeta::string_to_tm(std::string timeStr, tm &time)
    {
        int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec =0;

        if(sscanf(timeStr.c_str(), "%4d%2d%2dT%2d%2d%2d",
            &year, &month, &day, &hour, &minute, &sec) != 6)
        {
            return false;
        } 

        time.tm_year = year - 1900;
        time.tm_mon = month - 1;
        time.tm_mday = day;
        time.tm_hour = hour;
        time.tm_min = minute;
        time.tm_sec = sec;

        return true;
    }

    bool ContentMeta::string_to_time(std::string timeStr, time_t &time)
    {
        tm tm1;
        memset(&tm1, 0, sizeof(tm1));
        
        if(string_to_tm(timeStr, tm1))
        {
            time = mktime(&tm1);
        }
    }

    // 提取时间段元素据
    void ContentMeta::extractTSDesc(std::string startTimeStr, std::string endTimeStr, 
                            time_t inteval, std::string reportTimeStr)
    {
        
        tm reportT, startT, endT;
     
        string_to_tm(startTimeStr, startT);
        string_to_tm(endTimeStr, endT);
        string_to_tm(reportTimeStr, reportT);

        // 创建连续时间段描述符
        struct TSDesc tsDesc = {
            reportT,
            startT,
            endT,
            inteval
        };

        // 将连续时间段加入到时间中
        // 注意：这里日后需要排序。。。
        int tsID = this->desc->tDesc.tsDesc.size() + 1;
        this->desc->tDesc.tsID.push_back(tsID);
        this->desc->tDesc.tsDesc.push_back(tsDesc);
    }

    // 提取变量描述符
    void ContentMeta::extractVLDesc(std::string pathName)
    {
        // openNC

        // 循环读取变量，填充变量描述符

        while(1)
        {
            struct VarDesc var = {
                std::string("T2D"),
                3,
                0.01,
                std::string("double"),
                {0, 334, 334},
                0,
                1
            };

            int varID = this->desc->vDesc.vlDesc.size() + 1;

            this->desc->vDesc.varID.push_back(varID);
            this->desc->vDesc.vlDesc.push_back(var);

        }

        // 关闭NC


    }





}