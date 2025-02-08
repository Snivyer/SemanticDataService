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
#include "abstract/meta/cnt_ID.h"
#include "abstract/adaptor/local_adaptor.h"
#include "abstract/utils/time_operation.h"
#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>
#include <algorithm>

namespace SDS
{
    
    namespace fs = std::experimental::filesystem;

    // 内容元数据管理类
    class  ContentMeta: public Metadata
    {
    public:
        ContentMeta();
        ~ContentMeta();

        /*------提取元数据------*/
        // extract the semantic space desciption according to province, city and district.
        bool extractSSDesc(SSDesc &ssDesc, std::string province, std::string city = "0", std::string district = "0");   

        // extract the semantic space desciption according to arbitrary number of geoNames
        bool extractSSDesc(SSDesc &ssDesc, std::vector<std::string> &geoNames);

        
        // extract the time slot desciption according to directory name
        bool extractTSDesc(TSDesc &tsDesc, std::string dirName);
 

        // extract the time slot description according to time vector
        bool extractTSDesc(TSDesc &tsDesc, std::vector<std::string> &times);

        // extract the var list desciption by user give variable
        bool extractVLDesc(VLDesc &vlDesc, std::vector<std::string> &vars,
                                std::unordered_map<std::string, size_t> &varList);
        
        // extract the var list desciption by reading variable information from NetCDF 
        bool extractVLDesc(VLDesc &vlDesc, std::string dirName, bool isSame = true);


        /*------配置元数据存储------*/
        // set the schema of the table of the semantic space description 
        bool setSSDescFormat();

        // set the schema of the table of the time slot description 
        bool setTSDescFormat();

        // set the schema of the table of the var list description 
        bool setVLDescFormat();

        
        /*------填充元数据------*/
        // put the semantic space description
        bool putSSDesc(SSDesc &ssDesc, std::string spaceName = "", std::string spaceID ="");

        // put the time slot description 
        bool putTSDesc(TSDesc &tsDesc, std::string timeID = "");

        // put the var list description 
        bool putVLDesc(VLDesc &vlDesc, std::string VLID = "", std::string VlName = "");


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

        // parse the var description from query result of duckdb
        bool parseVarDesc(ContentDesc &cntDesc, duckdb::MaterializedQueryResult *result);      

        bool parseVLDesc(std::vector<ContentDesc> &cntDescSet); 

        
        /*------打印元数据------*/
        // print the semantic space description
        void printSSDesc(SSDesc &ssDesc);

        // print the time slot description
        void printTSDesc(TSDesc &tsDesc);
            
        // print the var list description
        void printVLDesc(VLDesc &vlDesc);
            


        /*------获取元数据------*/

        /*------持久化元数据------*/
        bool putMetaWithJson(std::string path) override;
        bool putMetaWithCSV(std::string path) override;
        bool putMetaWithParquet(std::string path) override;   




    private:
        std::string getMeta(Key key) override;
        void putMeta(Key key, std::string value) override;   


    };


}

