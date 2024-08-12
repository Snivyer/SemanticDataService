#include "abstract/GIS/geo_read.h"


namespace SDS {

json toJson(std::ifstream& ifs) {  
    json j; 
    ifs >> j;  
    return j;  
}

int SearchIndex(json J, const std::string &name){
    int index = -1;
    json subj = J["features"];

    //遍历json类中的对象
    int sz1 = subj.size();
       for (int i = 0; i < sz1; i++) {
        if (subj[i]["properties"]["name"] == name) {
            index = subj[i]["properties"]["subFeatureIndex"];
            return index;
        }
    }
    return index;
}

// 根据编码打开对应的json文件
json openJson(std::ifstream&ifs, const std::string& citycode) {
  std::string path = "./mapjson/" + citycode + ".json";
  ifs.open(path.c_str(), std::ios::in);
  if (!ifs.is_open()) {  
    std::cout << "Failed to open the file." << std::endl;  
  }
  return toJson(ifs);
}

void getJsonInfo(struct SSDesc &desc, json J, int index) {

    json subj = J["features"][index];
    desc.geoCentral.logitude = subj["properties"]["center"][0];
    desc.geoCentral.latitude = subj["properties"]["center"][1];
    desc.geoName = subj["properties"]["name"];       
    desc.adCode = subj["properties"]["adcode"].dump(); 
    getMinboundingRectangle(subj, desc.geoPerimeter);
}

bool getJsonInfo(struct SSDesc &desc, std::string &provice, std::string &city, std::string &district) {
    
    std::ifstream ifs("./mapjson/100000.json", std::ios::in) ; 
    std::ifstream ifsp, ifsc;
    std::string adcode;

    int index = 0;
    json J = toJson(ifs); 
    index = SearchIndex(J, provice);
    if(index == -1) {
      std::cout << "  省份输入错误！" << std::endl;
	    ifs.close();
      return false;
    }

    // 解析只输入省的情况
    if (city == "0" && district == "0") {
        getJsonInfo(desc, J, index);
        ifs.close();
        return true;
    }

     // 拿到市的行政编码
    adcode = J["features"][index]["properties"]["adcode"].dump();
    ifs.close();

    J = openJson(ifsp, adcode);
    index = SearchIndex(J, city);
    if(index == -1) {
        std::cout << "  地级市输入错误！" << std::endl;
        ifsp.close();
        return false;
    }

    // 解析输入省、市的情况
    if  (district == "0") {
        getJsonInfo(desc, J, index);
        ifsp.close();
        return true;
    }

    // 拿到县的行政编码
    adcode = J["features"][index]["properties"]["adcode"].dump();
    ifsp.close();

    J = openJson(ifsc, adcode);
    index = SearchIndex(J, district);
    if(index == -1) {
        std::cout << "  区县输入错误！" << std::endl;
        ifsc.close();
        return false;
    }

    getJsonInfo(desc, J, index);
    ifsc.close();
    return true;
}

bool getMinboundingRectangle(json J, std::vector<GeoCoordinate> &geoPerimeter) {

    double Maxlongitude = 0;            //最大经度
    double Maxlatitude = 0;             //最大纬度
    double Minlongitude = 180;          //最小经度
    double Minlatitude = 90;            //最小纬度


    int sz1 = J["geometry"]["coordinates"].size(), sz2 = 0, sz3 = 0;
    for (int i = 0; i < sz1; i++) {

      sz2 =  J["geometry"]["coordinates"][i].size();
      for (int j = 0; j <  sz2; j++) {

        sz3 = J["geometry"]["coordinates"][i][j].size();
        for(int k = 0; k <  sz3; k++){

            if (Maxlongitude < J["geometry"]["coordinates"][i][j][k][0]) {
              Maxlongitude = J["geometry"]["coordinates"][i][j][k][0];
            }
            if (Maxlatitude < J["geometry"]["coordinates"][i][j][k][1]) {
              Maxlatitude = J["geometry"]["coordinates"][i][j][k][1];
            }
            if (Minlongitude > J["geometry"]["coordinates"][i][j][k][0]) {
              Minlongitude = J["geometry"]["coordinates"][i][j][k][0];
            }
            if (Minlatitude > J["geometry"]["coordinates"][i][j][k][1]) {
              Minlatitude = J["geometry"]["coordinates"][i][j][k][1];
            }
        }
      }
    }

    GeoCoordinate v1, v2, v3, v4;
    v1.logitude = Maxlongitude;
    v1.latitude = Maxlatitude;
    v2.logitude = Maxlongitude;
    v2.latitude = Minlatitude;
    v3.logitude = Minlongitude;
    v3.latitude = Minlatitude;
    v4.logitude = Minlongitude;
    v4.latitude = Maxlatitude;
    
    geoPerimeter.push_back(v1);
    geoPerimeter.push_back(v2);
    geoPerimeter.push_back(v3);
    geoPerimeter.push_back(v4);
    return true;
}




void loadTable(MetaStore* &ms) {

  std::vector<std::string> files;
  files.push_back("100000.json");
  files.push_back("360000.json");
  files.push_back("360700.json");
  files.push_back("360800.json");

  std::string sql = "CREATE TABLE SSDESC (geoName VARCHAR, adCode VARCHAR, geoCentral DOUBLE[2], geoPerimeter DOUBLE[2][])";
  
  ms->excuteNonQuery(sql);
  for (auto item : files) {

      std::string filePath = "./mapjson/" + item;
      sql = "INSERT INTO SSDESC BY NAME ( SELECT unnest(features[1:-1]).properties.name AS geoName, unnest(features[1:-1]).properties.adcode AS adCode, unnest(features[1:-1]).properties.center AS geoCentral, unnest(features[1:-1]).geometry.coordinates[1][1] as geoPerimeter FROM '" + filePath + "')";
      ms->excuteNonQuery(sql);
  }

}

void getTableInfo(struct SSDesc &desc, const std::string &spaceName) {



}




}