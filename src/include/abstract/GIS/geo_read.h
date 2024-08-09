/* Author: Huang
*  Modifier: Snivyer
*  Create on: 2024/3/24
*  Description:
   Read geographic information from GIS json files
*/
#pragma once

#include <string> 
#include <vector>
#include <iostream>
#include <fstream>
#include "manager/cnt_meta.h"
#include "abstract/meta/meta_store.h"
#include "utils/string_operation.h"
#include "utils/json.hpp"


using json = nlohmann::json;  


namespace SDS   {
   
    //从文件流转成json对象  
    json toJson(std::ifstream& ifs);

    //搜索并返回地区的下标
    int SearchIndex(json J, const std::string &name);

    //打开对应编码的json文件
    json openJson(std::ifstream&ifs, const std::string& citycode);

    //获取最小边界矩形
    bool getMinboundingRectangle(json J, std::vector<GeoCoordinate> &geoPerimeter);

     // 根据省市县名获取地理空间描述富等信息，并将其存入空间结构体中
    bool getJsonInfo(struct SSDesc &desc, const std::string &provice, const std::string &city, const std::string &district);

    void getJsonInfo(struct SSDesc &desc, json J, int index);

    void loadTable(MetaStore* &ms);

    void getTableInfo(struct SSDesc &desc, const std::string &spaceName);

}

