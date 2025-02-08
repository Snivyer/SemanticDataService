#include "manager/metadata/cnt_meta.h"

namespace SDS {

    ContentMeta::ContentMeta() {

    }

    ContentMeta::~ContentMeta() {
    }


    bool ContentMeta::extractSSDesc(SSDesc &ssDesc, std::vector<std::string> &geoNames) {

        // todo: 由于现在只有三级的GIS文件，所有只能做到三级的查询，一旦拥有更多的GIS文件，可拓展该接口
        if(geoNames.size() == 3) {
            return extractSSDesc(ssDesc, geoNames[0], geoNames[1], geoNames[2]);
        } else if(geoNames.size() == 2) {
            return extractSSDesc(ssDesc, geoNames[0], geoNames[1]);
        } else if (geoNames.size() == 1) {
            return extractSSDesc(ssDesc, geoNames[0]);
        } else {
            return false;
        }
    }


    bool ContentMeta::extractSSDesc(SSDesc &ssDesc, std::string province, std::string city, std::string district) {
        try {
            getJsonInfo(ssDesc, province, city, district);
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


    bool ContentMeta::putSSDesc(SSDesc &ssDesc, std::string spaceName, std::string spaceID) {
    
        MetaStore* ms = getMetaStore();
        if(!ms) {
            return false;
        }

        std::stringstream fmt;
        fmt << "INSERT INTO SSDESC VALUES ('"   << spaceID << "','" 
                                                << spaceName << "','" 
                                                << ssDesc.geoName << "','" 
                                                << ssDesc.adCode << "', ["
                                                << ssDesc.geoCentral.logitude << ","
                                                << ssDesc.geoCentral.latitude << "], [[" 
                                                << ssDesc.geoPerimeter[0].logitude << ","
                                                << ssDesc.geoPerimeter[0].latitude << "],["
                                                << ssDesc.geoPerimeter[1].logitude << ","
                                                << ssDesc.geoPerimeter[1].latitude << "],["
                                                << ssDesc.geoPerimeter[2].logitude << ","
                                                << ssDesc.geoPerimeter[2].latitude << "],["
                                                << ssDesc.geoPerimeter[3].logitude << ","
                                                << ssDesc.geoPerimeter[3].latitude << "]])";
        return ms->excuteNonQuery(fmt.str());
    }

    bool ContentMeta::putTSDesc(TSDesc &tsDesc, std::string timeID) {
        MetaStore* ms = getMetaStore();
        if(!ms) {
            return false;
        }

        std::string reportT, startT, endT;
        tm_to_string(tsDesc.reportT, reportT);
        tm_to_string(tsDesc.startT, startT);
        tm_to_string(tsDesc.endT, endT);

        std::stringstream fmt;
        fmt << "INSERT INTO TSDESC VALUES ('"   << timeID << "','" 
                                                << reportT << "','" 
                                                << startT << "','" 
                                                << endT << "',"
                                                << tsDesc.interval << ","
                                                << tsDesc.count << ")";
        return ms->excuteNonQuery(fmt.str());
    }

    bool ContentMeta::putVLDesc(VLDesc  &vlDesc, std::string VLID, std::string VLName) {
        MetaStore* ms = getMetaStore();
        if(!ms) {
            return false;
        }

        for(int i = 0; i < vlDesc.groupLen; i++) {
            std::stringstream fmt;
            fmt << "INSERT INTO VLDESC VALUES ('" << VLID << "','" 
                                                << VLName << "'," 
                                                << i << ",'" 
                                                << vlDesc.desc[i].varName << "'," 
                                                << vlDesc.desc[i].varLen << "," 
                                                << vlDesc.desc[i].resRation << ",'"
                                                << vlDesc.desc[i].varType << "','"
                                                << vlDesc.desc[i].shapeTostr() << "',"
                                                << vlDesc.desc[i].ncVarID << ","
                                                << vlDesc.desc[i].ncGroupID << ")";
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


    // extract the time slot desciption accroding to directory name
    bool ContentMeta::extractTSDesc(TSDesc &tsDesc, std::string dirName) {

        std::vector<tm> tmList;
        FilePathList pathList;
        Adaptor* adaptor = getAdaptor();

        if(!adaptor) {
            return false;
        }

        std::string dirPath = combinePath(adaptor->connConfig.rootPath, dirName);
        if(adaptor->getFilePath(pathList, dirPath)) {
            for(auto fileName : pathList.fileNames) {
                tm fileTm;
                string_to_tm(splitString(fileName, '.')[1], fileTm);
                tmList.push_back(fileTm);
            }

            if(tmList.size() > 0) {
                std::sort(tmList.begin(), tmList.end(), compareTm);
                tsDesc.startT = *tmList.begin();
                tsDesc.endT = *(tmList.end() -1);

                tm secondFileTm = *(tmList.begin() + 1);
                tsDesc.interval = mktime(&(secondFileTm)) - mktime(&(tsDesc.startT));
                tsDesc.count = tmList.size();
            } else {
                tsDesc.count = 0;
            }

            // set the report Time by parse the directory name
            std::vector<std::string> dirNames = splitString(dirPath, '-');
            tm reportTm;
            string_to_tm(dirNames[1], reportTm);
            tsDesc.reportT = reportTm;
            return true;
        }
        return false;
    }

    bool ContentMeta::extractTSDesc(TSDesc &tsDesc, std::vector<std::string> &times) {
    
        if(times.size() == 0) {
            return false;
        }

        if(times.size() == 4) {
            // have set the interval
            time_t interval;
            string_to_time(times[3], interval);
            tsDesc.interval = interval;
        } 
    
        if (times.size() >= 3) {
            // have set the end time
            tm endT;
            string_to_tm(times[2], endT);
            tsDesc.endT = endT;
        } 
    
        if (times.size() >= 2) {
            // have set the start time
            tm startT;
            string_to_tm(times[1], startT);
            tsDesc.startT = startT;

            // count the time num
            if(times.size() == 4) {
                auto totalInterval = mktime(&(tsDesc.endT)) - mktime(&(tsDesc.startT));
                int count = totalInterval / tsDesc.interval;
                tsDesc.count = count;
            }
        }

        if (times.size() >= 1) {
            // just have set the report time
            tm reportT;
            string_to_tm(times[0], reportT);
            tsDesc.reportT = reportT;
        }
        return true;
    }

    bool ContentMeta::extractVLDesc(VLDesc &vlDesc, std::vector<std::string> &vars,
                                std::unordered_map<std::string, size_t> &varList) {
        for(auto varName : vars) {
            auto ret = varList.find(varName);
            if(ret != varList.end()) {
                vlDesc.varID.insert({varName, ret->second});
            }
        }

        vlDesc.groupLen = vlDesc.varID.size();
        if(vlDesc.groupLen == 0) {
            return false;
        }
        return true;
    }

    bool ContentMeta::extractVLDesc(VLDesc &vlDesc, std::string dirName, bool isSame) {
        Adaptor* adaptor;
        adaptor = getAdaptor();
        if(!adaptor) {
            return false;
        }

        FilePathList pathList;
        std::string dirPath = combinePath(adaptor->connConfig.rootPath, dirName);
        if(adaptor->getFilePath(pathList, dirPath)) {
            if(adaptor->getVarDescList(pathList, vlDesc.desc, isSame)) {
                vlDesc.groupLen =  vlDesc.desc.size();
                for(int i = 0; i < vlDesc.groupLen; i++) {
                    vlDesc.varID.insert({vlDesc.desc[i].varName, i});
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
    void ContentMeta::printSSDesc(SSDesc &ssDesc)  {
        ssDesc.print();
    }

    // print the time slot description
    void ContentMeta::printTSDesc(TSDesc &tsDesc) {
        tsDesc.print();
    }

    void ContentMeta::printVLDesc(VLDesc &vlDesc) {
        vlDesc.print();
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