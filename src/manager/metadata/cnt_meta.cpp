#include "manager/cnt_meta.h"



namespace SDS {

ContentMeta::ContentMeta() {

    

 
}

ContentMeta::~ContentMeta() {

}


bool ContentMeta::extractSSDesc(ContentDesc &cntDesc, std::vector<std::string> &geoNames) {

    // todo: 由于现在只有三级的GIS文件，所有只能做到三级的查询，一旦拥有更多的GIS文件，可拓展该接口
    if(geoNames.size() == 3) {
        return extractSSDesc(cntDesc, geoNames[0], geoNames[1], geoNames[2]);
    } else if(geoNames.size() == 2) {
        return extractSSDesc(cntDesc, geoNames[0], geoNames[1]);
    } else if (geoNames.size() == 1) {
        return extractSSDesc(cntDesc, geoNames[0]);
    } else {
        return false;
    }

}


bool ContentMeta::extractSSDesc(ContentDesc &cntDesc, std::string province, std::string city, std::string district) {
    try {
        getJsonInfo(cntDesc.ssDesc, province, city, district);
    } catch(Exception e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}


bool ContentMeta::setSSDescFormat() {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::string sql = "CREATE TABLE SSDESC (spaceID VARCHAR, spaceName VARCHAR, geoName VARCHAR, adCode VARCHAR, geoCentral DOUBLE[2], geoPerimeter DOUBLE[2][4])";
    return ms->excuteNonQuery(sql);
}

 bool ContentMeta::setTSDescFormat() {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::string sql = "CREATE TABLE TSDESC (TimeID VARCHAR, reportT TIMESTAMP_S, startT TIMESTAMP_S, endT TIMESTAMP_S, interval INTEGER, count INTEGER)";
    return ms->excuteNonQuery(sql);

}


bool ContentMeta::setVLDescFormat() {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::string sql = "CREATE TABLE VLDESC (VLID VARCHAR, VLName VARCHAR, VarID INTEGER, VarName VARCHAR, varLen INTEGER, resRation DOUBLE, varType VARCHAR, shape VARCHAR, ncVarID INTEGER, ncGroupID INTEGER)";
    return ms->excuteNonQuery(sql);

}


bool ContentMeta::putSSDesc(struct ContentDesc &desc, std::string spaceName, std::string spaceID) {
    
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::stringstream fmt;
    fmt << "INSERT INTO SSDESC VALUES ('"   << spaceID << "','" 
                                            << spaceName << "','" 
                                            << desc.ssDesc.geoName << "','" 
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

bool ContentMeta::putTSDesc(struct ContentDesc &desc, std::string timeID) {
    
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    std::string reportT, startT, endT;
    tm_to_string(desc.tsDesc.reportT, reportT);
    tm_to_string(desc.tsDesc.startT, startT);
    tm_to_string(desc.tsDesc.endT, endT);


    std::stringstream fmt;
    fmt << "INSERT INTO TSDESC VALUES ('"   << timeID << "','" 
                                            << reportT << "','" 
                                            << startT << "','" 
                                            << endT << "',"
                                            << desc.tsDesc.interval << ","
                                            << desc.tsDesc.count << ")";
    return ms->excuteNonQuery(fmt.str());

}

bool ContentMeta::putVLDesc(struct ContentDesc &desc, std::string VLID, std::string VLName) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    }

    for(int i = 0; i < desc.vlDesc.groupLen; i++) {
        std::stringstream fmt;
        fmt << "INSERT INTO VLDESC VALUES ('" << VLID << "','" 
                                            << VLName << "'," 
                                            << i << ",'" 
                                            << desc.vlDesc.desc[i].varName << "'," 
                                            << desc.vlDesc.desc[i].varLen << "," 
                                            << desc.vlDesc.desc[i].resRation << ",'"
                                            << desc.vlDesc.desc[i].varType << "','"
                                            << desc.vlDesc.desc[i].shapeTostr() << "',"
                                            << desc.vlDesc.desc[i].ncVarID << ","
                                            << desc.vlDesc.desc[i].ncGroupID << ")";
        ms->excuteNonQuery(fmt.str());
    }
}





bool ContentMeta::loadSSDescWithFile(std::string fileName) {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::string sql = "COPY SSDESC FROM '" + fileName + "'";
    return ms->excuteNonQuery(sql);
}

bool ContentMeta::loadTSDescWithFile(std::string fileName) {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::string sql = "COPY TSDESC FROM '" + fileName + "'";
    return ms->excuteNonQuery(sql);

}

bool ContentMeta::loadVLDescWithFile(std::string fileName) {
    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    std::string sql = "COPY VLDESC FROM '" + fileName + "'";
    return ms->excuteNonQuery(sql);

}


bool ContentMeta::parseSSDesc(std::vector<ContentDesc> &cntDescSet, 
                                duckdb::MaterializedQueryResult *result) {
    duckdb::idx_t row_count = result->RowCount();
    for (idx_t row = 0; row < row_count; row++) {
        ContentDesc cntDesc;
        cntDesc.ssDesc.geoName = result->GetValue(2, row).GetValue<std::string>();
        cntDesc.ssDesc.adCode  =  result->GetValue(3, row).GetValue<std::string>();

        auto gcList = ListValue::GetChildren(result->GetValue(4, row));
        cntDesc.ssDesc.geoCentral.logitude = gcList[0].GetValue<double>();
        cntDesc.ssDesc.geoCentral.latitude = gcList[1].GetValue<double>();
        
        auto gpList = ListValue::GetChildren(result->GetValue(5, row));
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

bool ContentMeta::parseTSDesc(std::vector<ContentDesc> &cntDescSet, duckdb::MaterializedQueryResult *result) {
    duckdb::idx_t row_count = result->RowCount();
    for (idx_t row = 0; row < row_count; row++) {
        ContentDesc cntDesc;
        std::string reportTstr = result->GetValue(1, row).GetValue<std::string>();
        std::string startTstr = result->GetValue(2, row).GetValue<std::string>();
        std::string endTstr = result->GetValue(3, row).GetValue<std::string>();
        
        strptime(reportTstr.c_str(), "%Y-%m-%d %H:%M:%S", &cntDesc.tsDesc.reportT);
        strptime(startTstr.c_str(), "%Y-%m-%d %H:%M:%S", &cntDesc.tsDesc.startT);
        strptime(endTstr.c_str(),   "%Y-%m-%d %H:%M:%S", &cntDesc.tsDesc.endT);
        cntDesc.tsDesc.interval =  static_cast<time_t>(result->GetValue(4, row).GetValue<int>());
        cntDesc.tsDesc.count    =  result->GetValue(5, row).GetValue<int>();
        cntDescSet.push_back(cntDesc);
    }
    return true;
}

bool ContentMeta::parseVarDesc(ContentDesc &cntDesc, duckdb::MaterializedQueryResult *result) {
    duckdb::idx_t row_count = result->RowCount();
    for (idx_t row = 0; row < row_count; row++) {
        int varID = result->GetValue(2, row).GetValue<int>();
        VarDesc varDesc;
        varDesc.varName = result->GetValue(3, row).GetValue<std::string>();
        varDesc.varLen  =  result->GetValue(4, row).GetValue<int>();
        varDesc.resRation = result->GetValue(5, row).GetValue<double>();
        varDesc.varType = result->GetValue(6, row).GetValue<std::string>();
        std::string shapeStr = result->GetValue(7, row).GetValue<std::string>();
        varDesc.strToShape(shapeStr);
        varDesc.ncVarID = result->GetValue(8, row).GetValue<int>();
        varDesc.ncGroupID = result->GetValue(9, row).GetValue<int>();
        cntDesc.vlDesc.desc.push_back(varDesc);
        cntDesc.vlDesc.varID.insert({varDesc.varName, varID});
    }
    cntDesc.vlDesc.groupLen = (int)row_count;
    return true;
}


bool ContentMeta::parseVLDesc(std::vector<ContentDesc> &cntDescSet) {

    MetaStore* ms = getMetaStore();
    if(!ms) {
        return false;
    } 

    duckdb::unique_ptr<duckdb::MaterializedQueryResult> result;
    std::string sql = "SELECT DISTINCT VLID FROM VLDESC";
    ms->excuteQuery(sql, result);

    duckdb::idx_t row_count = result->RowCount();
    for (idx_t row = 0; row < row_count; row++) {
        std::string VLID = result->GetValue(0, row).GetValue<std::string>();
        sql = "SELECT * FROM VLDESC WHERE VLID = '" + VLID +"'";
        ms->excuteQuery(sql, result);
        ContentDesc cntDesc;
        parseVarDesc(cntDesc, result.get());
        cntDescSet.push_back(cntDesc);
    }
    return true;
}



bool ContentMeta::string_to_tm(std::string timeStr, tm &time) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec =0;

    if(sscanf(timeStr.c_str(), "%4d%2d%2dT%2d%2d%2d", &year, &month, &day, &hour, &minute, &sec) != 6) {
        return false;
    } 

    memset(&time, 0, sizeof(tm));

    time.tm_year = year - 1900;
    time.tm_mon = month - 1;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minute;
    time.tm_sec = sec;
    return true;
}

bool ContentMeta::string_to_time(std::string timeStr, time_t &time) {
    tm tm1;
    memset(&tm1, 0, sizeof(tm1));
        
    if(string_to_tm(timeStr, tm1)) {
            time = mktime(&tm1);
    }
}

bool ContentMeta::tm_to_string(struct tm &tm, std::string &timeStr) {
    char buffer[32];
    std::strftime(buffer, 32,  "%Y-%m-%d %H:%M:%S", &tm);
    timeStr = std::string(buffer);
}

bool compareTm(const tm& a, const tm &b) {

    if (a.tm_year != b.tm_year) {
        return a.tm_year < b.tm_year;
    } else if (a.tm_mon != b.tm_mon) {
        return a.tm_mon < b.tm_mon;
    } else if (a.tm_mday != b.tm_mday) {
        return a.tm_mday < b.tm_mday;
    } else if (a.tm_hour != b.tm_hour) {
        return a.tm_hour < b.tm_hour;
    } else if (a.tm_min != b.tm_min) {
        return a.tm_min < b.tm_min;
    } else {
        return a.tm_sec < b.tm_sec;
    }

}

// extract the time slot desciption accroding to directory name
bool ContentMeta::extractTSDesc(struct ContentDesc &cntDesc, std::string dirPath) {

    std::vector<tm> tmList;
    FilePathList pathList;
    Adaptor* adaptor = getAdaptor();

    if(!adaptor) {
        return false;
    }

    if(adaptor->getFilePath(pathList, dirPath)) {
        for(auto fileName : pathList.fileNames) {
            tm fileTm;
            string_to_tm(splitString(fileName, '.')[1], fileTm);
            tmList.push_back(fileTm);
        }

        if(tmList.size() > 0) {
            std::sort(tmList.begin(), tmList.end(), compareTm);
            cntDesc.tsDesc.startT = *tmList.begin();
            cntDesc.tsDesc.endT = *(tmList.end() -1);

            tm secondFileTm = *(tmList.begin() + 1);
            cntDesc.tsDesc.interval = mktime(&(secondFileTm)) - mktime(&(cntDesc.tsDesc.startT));
            cntDesc.tsDesc.count = tmList.size();
        } else {
            cntDesc.tsDesc.count = 0;
        }

        // set the report Time by parse the directory name
        std::vector<std::string> dirNames = splitString(dirPath, '-');
        tm reportTm;
        string_to_tm(dirNames[1], reportTm);
        cntDesc.tsDesc.reportT = reportTm;
        return true;
    }
    return false;
}

 bool ContentMeta::extractVLDesc(struct ContentDesc &cntDesc, std::string dirPath, bool isSame) {

    Adaptor* adaptor;
    adaptor = getAdaptor();

    if(!adaptor) {
        return false;
    }

    FilePathList pathList;
    if(adaptor->getFilePath(pathList, dirPath)) {
        if(adaptor->getVarDescList(pathList, cntDesc.vlDesc.desc, isSame)) {
            cntDesc.vlDesc.groupLen =  cntDesc.vlDesc.desc.size();
            for(int i = 0; i < cntDesc.vlDesc.groupLen; i++) {
                cntDesc.vlDesc.varID.insert({cntDesc.vlDesc.desc[i].varName, i});
            }
            return true;
        }
        return false;
    }
    return false;
}



std::string ContentMeta::getMeta(Key key) {

}


void ContentMeta::putMeta(Key key, std::string value) {


}    


// print the semantic space description
void ContentMeta::printSSDesc(struct ContentDesc &desc) {
    
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

// print the time slot description
void ContentMeta::printTSDesc(struct ContentDesc &cntDesc) {
   
    if(cntDesc.tsDesc.count > 0) {
        std::cout << "起报时间:" << std::put_time(&cntDesc.tsDesc.reportT,"%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "时间段开始时间:" << std::put_time(&cntDesc.tsDesc.startT,"%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "时间段结束时间:" << std::put_time(&cntDesc.tsDesc.endT,"%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "时间段间隔(秒):" << cntDesc.tsDesc.interval << std::endl;
        std::cout << "时间段中时间点数量:" << cntDesc.tsDesc.count << std::endl;
    } else {
        std::cout << "起报时间:" << std::put_time(&cntDesc.tsDesc.reportT,"%Y-%m-%d %H:%M:%S") << std::endl;
        std::cout << "时间段中时间点数量:" << cntDesc.tsDesc.count << std::endl;
    }
}

void ContentMeta::printVLDesc(struct ContentDesc &cntDesc) {

    std::cout << "变量组内变量数量:" << cntDesc.vlDesc.groupLen << std::endl;

    for(int i = 0; i < cntDesc.vlDesc.groupLen; i++) {
        std::cout << "变量ID:" << i << std::endl;
        std::cout << "变量名:" << cntDesc.vlDesc.desc[i].varName << std::endl;
        std::cout << "变量长度:" << cntDesc.vlDesc.desc[i].varLen << std::endl;
        std::cout << "分辨率大小:" << cntDesc.vlDesc.desc[i].resRation << std::endl;
        std::cout << "变量类型:" << cntDesc.vlDesc.desc[i].varType << std::endl;
        std::cout << "变量维度:" << cntDesc.vlDesc.desc[i].shapeTostr() << std::endl;;
        std::cout << "NC文件ID:" << cntDesc.vlDesc.desc[i].ncVarID << std::endl;
        std::cout << "NC文件组ID:" << cntDesc.vlDesc.desc[i].ncGroupID << std::endl;
        std::cout << "---------------------------------" << cntDesc.vlDesc.desc[i].ncGroupID << std::endl;
    }
}

ContentDesc ContentMeta::getCntMeta(ContentID &cntID) {

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

    // put space metadata
    std::string fileName = path + "SSDesc/" + std::string(buffer) + ".json'";
    std::string sql = "COPY SSDESC TO '" + fileName;
    ms->excuteNonQuery(sql);

    // put time slot metadata
    fileName = path + "TSDesc/" + std::string(buffer) + ".json'";
    sql = "COPY TSDESC TO '" + fileName;
    ms->excuteNonQuery(sql);

    // put time slot metadata
    fileName = path + "VLDesc/" + std::string(buffer) + ".json'";
    sql = "COPY VLDESC TO '" + fileName;
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

    std::string fileName = path + "SSDesc/" + std::string(buffer) + ".csv";
    std::string sql = "COPY SSDESC TO '" + fileName + "' WITH (HEADER true)";
    ms->excuteNonQuery(sql);

    fileName = path + "TSDesc/" + std::string(buffer) + ".csv";
    sql = "COPY TSDESC TO '" + fileName + "' WITH (HEADER true)";
    ms->excuteNonQuery(sql);

    fileName = path + "VLDesc/" + std::string(buffer) + ".csv";
    sql = "COPY VLDESC TO '" + fileName + "' WITH (HEADER true)";
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

    std::string fileName = path + "SSDesc/" + std::string(buffer) + ".parquet";
    std::string sql = "COPY SSDESC TO '" + fileName + "' (FORMAT 'parquet')";
    ms->excuteNonQuery(sql);

    fileName = path + "TSDesc/" + std::string(buffer) + ".parquet";
    sql = "COPY TSDESC TO '" + fileName + "' (FORMAT 'parquet')";
    ms->excuteNonQuery(sql);

    fileName = path + "VLDesc/" + std::string(buffer) + ".parquet";
    sql = "COPY VLDESC TO '" + fileName + "' (FORMAT 'parquet')";
    return ms->excuteNonQuery(sql); 

} 





}