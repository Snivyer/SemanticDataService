/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   ContentMetadata is a children class of Metadata class, which is reponsible for management of the
   content metadata and construction of content index.
*/

#pragma once


#include "meta.h"
#include "abstract/GIS/geo_read.h"
#include "abstract/utils/string_operation.h"
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/adaptor/local_adaptor.h"
#include <string>
#include <vector>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <experimental/filesystem>
#include <algorithm>

namespace SDS
{
    
    namespace fs = std::experimental::filesystem;
    bool compareTm(const tm& a, const tm &b);

    // 内容元数据管理类
    class  ContentMeta: public Metadata
    {
    public:
        ContentMeta();
        ~ContentMeta();

        /*------提取元数据------*/
        // extract the semantic space desciption according to province, city and district.
        bool extractSSDesc(struct ContentDesc &cntDesc, std::string province, std::string city = "0", std::string district = "0");   

        // extract the semantic space desciption according to arbitrary number of geoNames
        bool extractSSDesc(struct ContentDesc &cntDesc, std::vector<std::string> &geoNames);

        
        // extract the time slot desciption accroding to directory name
        bool extractTSDesc(struct ContentDesc &cntDesc, std::string dirName);  

    
        // extract the var list desciption by reading variable information fromNetCDF 
        bool extractVLDesc(struct ContentDesc &cntDesc, std::string dirName, bool isSame = true);


        /*------配置元数据存储------*/
        // set the schema of the table of the semantic space description 
        bool setSSDescFormat();

        // set the schema of the table of the time slot description 
        bool setTSDescFormat();

        // set the schema of the table of the var list description 
        bool setVLDescFormat();

        
        /*------填充元数据------*/
        // put the semantic space description
        bool putSSDesc(struct ContentDesc &cntDesc, std::string spaceName = "", std::string spaceID ="");

        // put the time slot description 
        bool putTSDesc(struct ContentDesc &cntDesc, std::string timeID = "");

        // put the var list description 
        bool putVLDesc(struct ContentDesc &cntDesc, std::string VLID = "", std::string VlName = "");


        /*------加载元数据------*/
        // load the semantic space description from CSV/Json/Parquet
        bool loadSSDescWithFile(std::string filePath);

        // load the time slot description from CSV/Json/Parquet
        bool loadTSDescWithFile(std::string filePath);

        // load the time slot description from CSV/Json/Parquet
        bool loadVLDescWithFile(std::string filePath);


        /*------解析元数据------*/
        // parse the semantic space description from query result of duckdb
        bool parseSSDesc(std::vector<ContentDesc> &cntDescSet,  duckdb::MaterializedQueryResult *result);

        // parse the time slot description from query result of duckdb
        bool parseTSDesc(std::vector<ContentDesc> &cntDescSet,  duckdb::MaterializedQueryResult *result);

        // parse the var list description from query result of duckdb
        bool parseVLDesc(std::vector<ContentDesc> &cntDescSet,  duckdb::MaterializedQueryResult *result);       

        
        /*------打印元数据------*/
        // print the semantic space description
        void printSSDesc(struct ContentDesc &cntDesc);

        // print the time slot description
        void printTSDesc(struct ContentDesc &cntDesc);

        // print the var list description
        void printVLDesc(struct ContentDesc &cntDesc);


        /*------获取元数据------*/
        // get content metadata by search their cntID
        ContentDesc getCntMeta(ContentID &cntID);

        /*------持久化元数据------*/
        bool putMetaWithJson(std::string path) override;
        bool putMetaWithCSV(std::string path) override;
        bool putMetaWithParquet(std::string path) override;   


    private:
        bool string_to_tm(std::string timeStr, struct tm &time);       
        bool string_to_time(std::string timeStr, time_t &time); 
        bool tm_to_string(struct tm &tm, std::string &timeStr);
     



        std::string getMeta(Key key) override;
        void putMeta(Key key, std::string value) override;   


    };


}

