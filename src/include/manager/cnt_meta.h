/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   ContentMetadata is a children class of Metadata class, which is reponsible for management of the
   content metadata and construction of content index.
*/

#pragma once


#include "meta.h"
#include "abstract/GIS/geo_read.h"
#include "abstract/meta/cnt_meta_template.h"
#include <string>
#include <vector>
#include <sys/time.h>
#include <string.h>
#include <iostream>

namespace SDS
{

    // 内容元数据管理类
    class ContentMeta: public Metadata
    {
    public:
        ContentMeta();
        ~ContentMeta();

        /*------提取元数据------*/
        // 根据省、市、县提取空间描述符
        void extractSSDesc(struct ContentDesc &cntDesc, std::string province, std::string city = "0", std::string district = "0");   

        // extract the space desciption according to arbitrary number of geoNames
        void extractSSDesc(struct ContentDesc &cntDesc, std::vector<std::string> &geoNames);
        
        // set the schema of the table of the semantic space description 
        bool setSSDescFormat();

        // put the semantic space description
        bool putSSDesc(struct ContentDesc &cntDesc);

        // load the content space description from CSV/Json/Parquet
        bool loadSSDescWithFile(std::string filePath);


        // parse the content space description from query result of duckdb
        bool parseSSDesc(std::vector<ContentDesc> &cntDescSet,  duckdb::MaterializedQueryResult *result);

        // print the content space description
        void printSSDesc(struct ContentDesc &cntDesc);


        void extractTSDesc(struct ContentDesc &cntDesc, std::vector<std::string> filePathList);

        void extractTSDesc(struct ContentDesc &cntDesc, std::string startTime, std::string endTime,  // 根据文件名信息，提取时间段描述符
                        time_t inteval, std::string reportTimeString);

        void extractTSDesc(struct ContentDesc &cntDesc, std::string dirpath);  // 根据目录路径信息，提取时间段描述符;

        void extractVLDesc(struct ContentDesc &cntDesc, std::string filePath);      // 从NC文件中提取变量列表

        /*------获取元数据------*/
        ContentDesc getCntMeta(struct ContentID &cntID);

        

 

        
        bool putMetaWithJson(std::string path) override;
        bool putMetaWithCSV(std::string path) override;
        bool putMetaWithParquet(std::string path) override;   


    private:

    
        bool string_to_tm(std::string timeStr, struct tm &time);       
        bool string_to_time(std::string timeStr, time_t &time);  

        std::string getMeta(Key key) override;
        void putMeta(Key key, std::string value) override;   


    };


}

