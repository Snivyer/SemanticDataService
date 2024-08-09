

#include "manager/cnt_meta.h"
#include "abstract/GIS/geo_read.h"


namespace SDS {

ContentMeta::ContentMeta() {

    this->desc = new ContentDesc();
}

ContentMeta::~ContentMeta() {


    if(this->desc != nullptr) {
        delete this->desc;
    }

}


void ContentMeta::extractSSDesc(std::string province, std::string city, std::string district) {
    getJsonInfo(this->desc->ssDesc, province, city, district);
}

void ContentMeta::putSSDesc(MetaStore* &ms) {

    std::stringstream fmt;
    fmt << "INSERT INTO SSDESC VALUES ('"   << desc->ssDesc.geoName << "','" 
                                            << desc->ssDesc.adCode << "', ["
                                            << desc->ssDesc.geoCentral.logitude << ","
                                            << desc->ssDesc.geoCentral.latitude << "], [[" 
                                            << desc->ssDesc.geoPerimeter[0].logitude << ","
                                            << desc->ssDesc.geoPerimeter[0].latitude << "],["
                                            << desc->ssDesc.geoPerimeter[1].logitude << ","
                                            << desc->ssDesc.geoPerimeter[1].latitude << "],["
                                            << desc->ssDesc.geoPerimeter[2].logitude << ","
                                            << desc->ssDesc.geoPerimeter[2].latitude << "],["
                                            << desc->ssDesc.geoPerimeter[3].logitude << ","
                                            << desc->ssDesc.geoPerimeter[3].latitude << "]])";
    
    ms->excuteNonQuery(fmt.str());
}

bool ContentMeta::string_to_tm(std::string timeStr, tm &time) {
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
                            time_t inteval, std::string reportTimeStr) {

}

void ContentMeta::extractTSDesc(std::string dirpath) {


}


// 提取变量描述符
void ContentMeta::extractVLDesc(std::string pathName) {
      
}


std::string ContentMeta::getMeta(Key key) {

}


void ContentMeta::putMeta(Key key, std::string value) {

}    


void ContentMeta::printSSDesc() {
    
    std::cout << "  地理空间名字： " << desc->ssDesc.geoName << std::endl;
    std::cout << "  行政编码： " << desc->ssDesc.adCode << std::endl;
    std::cout << "  中心经纬度： " <<  desc->ssDesc.geoCentral.logitude <<" , "
                                  << desc->ssDesc.geoCentral.latitude << std::endl;

    std::cout << "  地理周界: "<< std::endl;
    for(int i = 0; i < desc->ssDesc.geoPerimeter.size(); i++){
            std::cout << "      " << desc->ssDesc.geoPerimeter[i].logitude
            <<" , "<< desc->ssDesc.geoPerimeter[i].latitude << std::endl; 
        }

}



void ContentMeta::putMetaWithJson(std::string path, MetaStore* &ms) {

    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32,  "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".json'";
    std::string sql = "COPY SSDESC TO '" + fileName;
    ms->excuteNonQuery(sql);

}

void ContentMeta::putMetaWithCSV(std::string path, MetaStore* &ms) {

    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".csv";
    std::string sql = "COPY SSDESC TO '" + fileName + "' WITH (HEADER true)";
    ms->excuteNonQuery(sql);

}

void ContentMeta::putMetaWithParquet(std::string path, MetaStore* &ms) {

    
    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".parquet";
    std::string sql = "COPY SSDESC TO '" + fileName + "' (FORMAT 'parquet')";
    ms->excuteNonQuery(sql);

} 





}