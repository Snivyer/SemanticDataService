#include "manager/cnt_meta.h"



namespace SDS {

ContentMeta::ContentMeta() {

    

 
}

ContentMeta::~ContentMeta() {

}


void ContentMeta::extractSSDesc(ContentDesc &cntDesc, std::vector<std::string> &geoNames) {

    // todo: 由于现在只有三级的GIS文件，所有只能做到三级的查询，一旦拥有更多的GIS文件，可拓展该接口
    if(geoNames.size() == 3) {
        extractSSDesc(cntDesc, geoNames[0], geoNames[1], geoNames[2]);
    } else if(geoNames.size() == 2) {
        extractSSDesc(cntDesc, geoNames[0], geoNames[1]);
    } else if (geoNames.size() == 1) {
        extractSSDesc(cntDesc, geoNames[0]);
    } else {
        int a = 0;

    }

}


void ContentMeta::extractSSDesc(ContentDesc &cntDesc, std::string province, std::string city, std::string district) {
    try {
        getJsonInfo(cntDesc.ssDesc, province, city, district);
    } catch(Exception e) {
        std::cout << e.what() << std::endl;
    }
}


bool ContentMeta::setSSDescFormat() {
    
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::string sql = "CREATE TABLE SSDESC (geoName VARCHAR, adCode VARCHAR, geoCentral DOUBLE[2], geoPerimeter DOUBLE[2][4])";
    return ms->excuteNonQuery(sql);

}

bool ContentMeta::putSSDesc(ContentDesc &desc) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::stringstream fmt;
    fmt << "INSERT INTO SSDESC VALUES ('"   << desc.ssDesc.geoName << "','" 
                                            << desc.ssDesc.adCode << "', ["
                                            << desc.ssDesc.geoCentral.logitude << ","
                                            << desc.ssDesc.geoCentral.latitude << "], [[" 
                                            << desc.ssDesc.geoPerimeter[0].logitude << ","
                                            << desc.ssDesc.geoPerimeter[0].latitude << "],["
                                            << desc.ssDesc.geoPerimeter[1].logitude << ","
                                            << desc.ssDesc.geoPerimeter[1].latitude << "],["
                                            << desc.ssDesc.geoPerimeter[2].logitude << ","
                                            << desc.ssDesc.geoPerimeter[2].latitude << "],["
                                            << desc.ssDesc.geoPerimeter[3].logitude << ","
                                            << desc.ssDesc.geoPerimeter[3].latitude << "]])";
    
    return ms->excuteNonQuery(fmt.str());
}

bool ContentMeta::loadSSDescWithFile(std::string fileName) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::string sql = "COPY SSDESC FROM '" + fileName + "'";
    return ms->excuteNonQuery(sql);
}


bool ContentMeta::parseSSDesc(std::vector<ContentDesc> &cntDescSet, 
                                duckdb::MaterializedQueryResult *result) {
    duckdb::idx_t row_count = result->RowCount();
    for (idx_t row = 0; row < row_count; row++) {
        ContentDesc cntDesc;
        cntDesc.ssDesc.geoName = result->GetValue(0, row).GetValue<std::string>();
        cntDesc.ssDesc.adCode  =  result->GetValue(1, row).GetValue<std::string>();

        auto gcList = ListValue::GetChildren(result->GetValue(2, row));
        cntDesc.ssDesc.geoCentral.logitude = gcList[0].GetValue<double>();
        cntDesc.ssDesc.geoCentral.latitude = gcList[1].GetValue<double>();
        
        auto gpList = ListValue::GetChildren(result->GetValue(3, row));
        for(auto item : gpList) {
            GeoCoordinate geoCoor;
            auto childList = ListValue::GetChildren(item);
            geoCoor.logitude = childList[0].GetValue<double>();
            geoCoor.latitude = childList[1].GetValue<double>();
            cntDesc.ssDesc.geoPerimeter.push_back(geoCoor);
        }
        cntDescSet.push_back(cntDesc);
    }
    return true;
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
void ContentMeta::extractTSDesc(ContentDesc &cntDesc, std::string startTimeStr, std::string endTimeStr, 
                            time_t inteval, std::string reportTimeStr) {

}


void ContentMeta::extractTSDesc(ContentDesc &cntDesc, std::vector<std::string> filePathList ) {

}



// 提取变量描述符
void ContentMeta::extractVLDesc(ContentDesc &cntDesc, std::string pathName) {
      
}


std::string ContentMeta::getMeta(Key key) {

}


void ContentMeta::putMeta(Key key, std::string value) {

}    


void ContentMeta::printSSDesc(ContentDesc &desc) {
    
    std::cout << "  地理空间名字： " << desc.ssDesc.geoName << std::endl;
    std::cout << "  行政编码： " << desc.ssDesc.adCode << std::endl;
    std::cout << "  中心经纬度： " <<  desc.ssDesc.geoCentral.logitude <<" , "
                                  << desc.ssDesc.geoCentral.latitude << std::endl;

    std::cout << "  地理周界: "<< std::endl;
    for(int i = 0; i < desc.ssDesc.geoPerimeter.size(); i++){
            std::cout << "      " << desc.ssDesc.geoPerimeter[i].logitude
            <<" , "<< desc.ssDesc.geoPerimeter[i].latitude << std::endl; 
        }

}



bool ContentMeta::putMetaWithJson(std::string path) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32,  "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".json'";
    std::string sql = "COPY SSDESC TO '" + fileName;
    return ms->excuteNonQuery(sql);

}

bool ContentMeta::putMetaWithCSV(std::string path) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".csv";
    std::string sql = "COPY SSDESC TO '" + fileName + "' WITH (HEADER true)";
    return ms->excuteNonQuery(sql);

}

bool ContentMeta::putMetaWithParquet(std::string path) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 
    
    std::time_t now = std::time(nullptr);
    struct std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d-%H-%M-%S", ptm);

    std::string fileName = path + std::string(buffer) + ".parquet";
    std::string sql = "COPY SSDESC TO '" + fileName + "' (FORMAT 'parquet')";
    return ms->excuteNonQuery(sql);

} 





}