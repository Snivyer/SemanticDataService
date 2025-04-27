/* Author: Huang
*  Modifier: Snivyer
*  Create on: 2025/3/24
*  Description:
   Read geographic information with gaode api
*/
#pragma once

#include <iostream>  
#include <string>  
#include <cstddef>
#include <map>  
#include <vector>  
#include <curl/curl.h> 
#include "simdjson.h"
#include <fstream>   
#include <iostream> 
#include "manager/metadata/cnt_meta.h"
#include "abstract/GIS/simdjson.h"

//限制测试API的发送周期
#include <chrono>    
#include <thread>
using namespace simdjson; 
const std::string gaode_api_key = "94e3aa9531d719fc08bb4892d01fb2cd"; 


namespace SDS   {

  // 存储选项信息  
    struct DistrictOption {  
        std::string name;  
        std::string center;  
        std::string adcode;
        simdjson::dom::element element;  
    }; 

    /**  
    * libcurl 数据写入回调函数  
    * @param ptr 接收到的数据指针  
    * @param size 数据块单位尺寸  
    * @param nmemb 数据块数量  
    * @param stream 用户自定义数据指针（此处为string缓冲区）  
    * @return 实际处理的数据量（应与size*nmemb一致）  
    */  
    size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* stream);

    // get spaceDesc by acode
    bool getSSDescByCode(const std::string& adcode, SSDesc& ssDescs);
    std::string getGeoNameByCodeWithGaode(const std::string& adcode);
    
  
    std::string GetGeoName(const std::string& districtName, std::string &adcode, int choice = -1);
    std::string getChinaSSDesc(std::map<std::string, SSDesc>& ssDesc);
    
    bool getMinboundingRectangle(simdjson::dom::element &district, std::vector<GeoCoordinate> &geoPerimeter);
    void chooseOnlyOne(std::vector<DistrictOption> &options, simdjson::dom::array &districts, simdjson::dom::element &selectedDistrict, int &choice);
}
