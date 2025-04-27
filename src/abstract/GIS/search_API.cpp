#include"abstract/GIS/search_API.h"
#include <curl/curl.h> 


namespace SDS {
 
    size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* stream) {  
        const size_t realsize = size * nmemb;  
        stream->append(ptr, realsize);  
        return realsize;  
    }     

    bool getSSDescByCode(const std::string& adcode, SSDesc& ssDesc) {  
        simdjson::dom::array districts; 
        CURL* curl = curl_easy_init();  
        if (!curl) {
            return "";  
        }
    
        std::string response;     
        const std::string url =   
            "https://restapi.amap.com/v3/config/district?keywords=" +   
            adcode +   
            "&showbiz=false" +   
            "&subdistrict=0&extensions=all&key=" + gaode_api_key;  
    
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);  
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);  
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);  
        //转换utf-8，有用？
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");  
        curl_easy_setopt(curl, CURLOPT_ENCODING, "UTF-8"); 
        CURLcode res = curl_easy_perform(curl);  
        curl_easy_cleanup(curl);  
    
        if (res == CURLE_OK) {  
            try {  
                simdjson::dom::parser parser;  
                simdjson::dom::element doc;  
                auto error = parser.parse(response).get(doc);  
                
                if (error) {  
                    std::cerr << "JSON解析失败: " << error << std::endl;  
                    return"";  
                }  
    
                // 检查状态和districts数组  
                std::string_view status;  
                if (doc["status"].get_string().get(status) != 0 || status != "1") {  
                    std::cout<<"错误原因："<<doc["info"].get_string()<<std::endl;
                    return"";  
                }  
    
                
                if (doc["districts"].get_array().get(districts) != 0 || districts.begin() == districts.end()) {  
                    std::cout<<"错误原因："<<doc["info"].get_string()<<std::endl;
                    return"";  
                }  
            } catch (const simdjson::simdjson_error& e) {
                std::cerr << "JSON解析错误: " << e.what() << std::endl;  
                return"";  
            } catch (const std::exception& e) {  
                std::cerr << "标准异常: " << e.what() << std::endl;  
                return"";  
            } catch (...) {  
                std::cerr << "未知异常" << std::endl;  
                return"";  
            }
        }
    
                // 获取第一个district  
        auto district = *districts.begin();  
        ssDesc.adCode = adcode;
        std::string_view name;  
        if (district["name"].get_string().get(name) == 0) {  
            ssDesc.geoName = std::string(name);
        } 

        // 获取polyline  
        std::string_view polyline;  
         if (district["polyline"].get_string().get(polyline) == 0) {  
             std::string_view remaining = polyline;  
             size_t pos;  
     
             // 分割字符串处理坐标点  
             while ((pos = remaining.find(';')) != std::string_view::npos) {  
                 std::string_view point_str = remaining.substr(0, pos);  
                 remaining = remaining.substr(pos + 1);  
 
                 size_t comma_pos = point_str.find(',');  
                 if (comma_pos != std::string_view::npos) {  
                     try {  
                         std::string lng_str(point_str.substr(0, comma_pos));  
                         std::string lat_str(point_str.substr(comma_pos + 1));  
                         double lng = std::stod(lng_str);  
                         double lat = std::stod(lat_str);   
                         ssDesc.geoPerimeter.push_back({lng, lat}); 
                     } catch (const std::exception& e) {  
                         continue;  
                     }  
                 }  
             }  
 
             if (!remaining.empty()) {  
                 size_t comma_pos = remaining.find(',');  
                 if (comma_pos != std::string_view::npos) {  
                     try {  
                         std::string lng_str(remaining.substr(0, comma_pos));  
                         std::string lat_str(remaining.substr(comma_pos + 1));  
                         double lng = std::stod(lng_str);  
                         double lat = std::stod(lat_str); 
                         ssDesc.geoPerimeter.push_back({lng, lat});  
                     } catch (const std::exception& e) {  
                         // 忽略解析错误  
                     }  
                 }  
             } 
             return true;   
            }
        return false; 
    }

    std::string getGeoNameByCodeWithGaode(const std::string& adcode) {  
        CURL* curl = curl_easy_init();  
        std::string response;  
    const std::string url =   
        "https://restapi.amap.com/v3/config/district?keywords=" +   
        adcode +   
        "&subdistrict=0&extensions=base&key="+ gaode_api_key;  

    if (curl) {  
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);  
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);  
        curl_easy_perform(curl);  
        curl_easy_cleanup(curl);  
    }  

    try {  
        // 创建解析器  
        simdjson::dom::parser parser;  
        
        // 解析JSON响应  
        simdjson::dom::element doc = parser.parse(response);  
        
        // 检查状态  
        std::string_view status;  
        auto error = doc["status"].get_string().get(status);  
        if (error || status != "1") {  
            return "";  
        }  
        
        // 获取districts数组  
        simdjson::dom::array districts;  
        error = doc["districts"].get_array().get(districts);  
        if (error || districts.begin() == districts.end()) {  
            return "";  
        }  
        
        // 获取第一个district的name  
        std::string_view name;  
        error = districts.at(0)["name"].get_string().get(name);  
        if (!error) {  
            return std::string(name);  
        }  
    }  
    catch (const simdjson::simdjson_error& e) {  
        std::cerr << "JSON解析错误: " << e.what() << std::endl;  
    }  
    catch (const std::exception& e) {  
        std::cerr << "标准异常: " << e.what() << std::endl;  
    }  
    catch (...) {  
        std::cerr << "未知异常" << std::endl;  
    }  

    return "";  
    } 
        
   
    void chooseOnlyOne(std::vector<DistrictOption> &options, simdjson::dom::array &districts,
                         simdjson::dom::element &selectedDistrict, int &choice) {
        std::cout << "找到多个匹配的区划，请选择一个：\n";  
        size_t index = 0;   
        for (simdjson::dom::element district : districts) {  
            std::string_view name, center, adcode;  
            if (district["name"].get(name) || district["center"].get(center) ||
                district["adcode"].get(adcode)) {  
                continue;  
            }  
            options.push_back({  
                std::string(name),  
                std::string(center),  
                std::string(adcode),
                district});  
            std::cout << ++index << ". " << name << " (中心点: " << center << ")\n";  
        }  

        if (options.empty()) {  
            return;  
        }  

        size_t userChoice = 0;  
        std::string input;  
        bool validInput = false;  
        while (!validInput) {  
            std::cout << "请输入选择的序号 (1-" << options.size() << "): ";  
            std::getline(std::cin >> std::ws, input);  
        
            bool isNumber = true;  
            for (char c : input) {  
                if (!std::isdigit(c)) {  
                    isNumber = false;  
                    break;  
                }  
            }  
        
            if (!isNumber) {  
                std::cout << "输入无效，请输入数字。\n";  
                continue;  
            }  
        
            try {  
                userChoice = std::stoul(input);  
                if (userChoice >= 1 && userChoice <= options.size()) {  
                    validInput = true;  
                    selectedDistrict = options[userChoice-1].element;  
                    choice = userChoice;
                } else {  
                    std::cout << "输入的序号超出范围，请重新输入。\n";  
                }  
            } catch (const std::exception&) {  
                std::cout << "输入无效，请输入有效的数字。\n";  
            }  
        }  
    }

    std::string GetGeoName(const std::string& districtName, std::string &objectAdcode, int choice) {  
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);  
        if (!curl) {  
            throw std::runtime_error("CURL初始化失败");  
        }  

        char* encodedName = curl_easy_escape(curl.get(), districtName.c_str(), 0);  
        if (!encodedName) {  
            throw std::runtime_error("区划名称编码失败");  
        }  
    
        std::string url = "https://restapi.amap.com/v3/config/district?keywords=" +   
                        std::string(encodedName) +   
                        "&subdistrict=0&extensions=base&key=" + gaode_api_key;  
        curl_free(encodedName);  

        std::string response;  
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());  
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, writeCallback);  
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);  
        curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5L);  
        
        // 添加这行，指定接受的编码  
        curl_easy_setopt(curl.get(), CURLOPT_ACCEPT_ENCODING, "");  
        curl_easy_setopt(curl.get(), CURLOPT_ENCODING, "UTF-8");  

        CURLcode res = curl_easy_perform(curl.get());  
        if (res != CURLE_OK) {  
            throw std::runtime_error("API请求失败: " +   
                               std::string(curl_easy_strerror(res)));  
        }  

        simdjson::dom::parser parser;  
        simdjson::dom::element doc;  
        auto error = parser.parse(response).get(doc);  
        if (error) {  
            throw std::runtime_error("JSON解析失败");  
        }  

        std::string_view status;  
        error = doc["status"].get(status);  
        if (error || status != "1") {  
            std::cout << "API状态检查失败：" << doc["info"].get_string() << std::endl; 
            return "";  
        }  

        simdjson::dom::array districts;  
        error = doc["districts"].get(districts);  
        if (error || districts.begin() == districts.end()) {  
            std::cout << "API状态检查失败：" << doc["info"].get_string() << std::endl; 
            return "";  
        }  

        simdjson::dom::element selectedDistrict;  
        if (districts.size() == 1) {  
            selectedDistrict = districts.at(0);  
        } else if(districts.size() != 1 && choice == -1){  
            
            // 多个结果的情况  
            std::cout << "找到多个匹配的区划，请选择一个：\n";  
            size_t index = 0;  
        
            // 存储每个选项的信息  
            struct DistrictOption {  
                std::string name;  
                std::string center;  
                std::string adcode;
                simdjson::dom::element element;  
            };  
            std::vector<DistrictOption> options;  

            for (simdjson::dom::element district : districts) {  
                std::string_view name, center, adcode;  
                if (district["name"].get(name) || district["center"].get(center)||
                   district["adcode"].get(adcode)) {  
                    continue;  
                }  
            
                options.push_back({  
                    std::string(name),  
                    std::string(center), 
                    std::string(adcode), 
                    district  
                });  
                std::cout << ++index << ". " << name <<" adcode："<<adcode<< " (中心点: " << center << ")\n";  
            }  

            if (options.empty()) {  
                return "";  
            }  

            size_t user_choice = 0;  
            std::string input;  
            bool validInput = false;  

            while (!validInput) {  
                std::cout << "请输入选择的序号 (1-" << options.size() << "): ";  
                std::getline(std::cin >> std::ws, input);  
            
                bool isNumber = true;  
                for (char c : input) {  
                    if (!std::isdigit(c)) {  
                        isNumber = false;  
                        break;  
                    }  
                }  
            
                if (!isNumber) {  
                    std::cout << "输入无效，请输入数字。\n";  
                    continue;  
                }  
            
            try {  
                user_choice = std::stol(input);
                
                if (user_choice >= 1 && user_choice <= options.size()) {  
                    validInput = true;  
                   
                    selectedDistrict = options[user_choice - 1].element;  
                    
                    choice = user_choice; //保存用户的选择
                    
                } else {  
                    std::cout << "输入的序号超出范围，请重新输入。\n";  
                }  
            } catch (const std::exception&) {  
                std::cout << "输入无效，请输入有效的数字。\n";  
            }  
        }  
    } else if(districts.size() != 1 && choice != -1) { 
        
        // 存储选项信息  
        struct DistrictOption {  
            std::string name;  
            std::string center;  
            std::string adcode;
            simdjson::dom::element element;  
        };  
        
        std::vector<DistrictOption> options;  

        for (simdjson::dom::element district : districts) {  
            std::string_view name, center, adcode;  
            if (district["name"].get(name) || district["center"].get(center)||
                   district["adcode"].get(adcode)) {  
                continue;  
            }  
            
            options.push_back({  
                std::string(name),  
                std::string(center), 
                std::string(adcode), 
                district  
            });  
        }  

        if (options.empty()) {  
            return "";  
        }   
        selectedDistrict = options[choice-1].element;  
    }

    // 处理选中的区划  
    std::string_view name_view, level_view;  
    std::string_view province_view, city_view;  
    
    if (selectedDistrict["name"].get(name_view) ||   
        selectedDistrict["level"].get(level_view)) {  
        return "";  
    }  

    // 转换为 std::string 以便长期存储和使用  
    std::string name(name_view);  
    std::string level(level_view);   


    //获取行政编码用来检索上级区划
    std::string adcode = std::string(selectedDistrict["adcode"].get_string().value());

    std::string fullPath = "中国";  
    
    if (level == "province") {  
        
        fullPath += "-" + std::string(name);  
    }   
    else if (level == "city") {  
        std::string  province_code = adcode.substr(0, 2) + "0000";
        std::string province = std::string(getGeoNameByCodeWithGaode(province_code));
        // 在API调用之间添加延时  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 延时500毫秒   
        fullPath += "-" + std::string(province) + "-" + std::string(name);  
    }   
    else if (level == "district") {  
        // 在API调用之间添加延时  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 延时500毫秒 

        std::string province_code = adcode.substr(0, 2) + "0000";
        std::string province = std::string(getGeoNameByCodeWithGaode(province_code));
        // 在API调用之间添加延时  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 延时500毫秒   

        std::string city_code = adcode.substr(0, 4) + "00"; 
        // 在API调用之间添加延时  
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 延时500毫秒 

        std::string city = std::string(getGeoNameByCodeWithGaode(city_code));

        fullPath += "-" + std::string(province) + "-" + std::string(city) + "-" + std::string(name);  
    }  
        return fullPath; 
}

    std::string getChinaSSDesc(std::map<std::string, SSDesc>& ssDescs)  {  
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);  
        if (!curl) {  
            throw std::runtime_error("CURL初始化失败");  
        }  

        std::string url = "https://restapi.amap.com/v3/config/district?keywords=100000&subdistrict=0&extensions=all&key=" + gaode_api_key;  
        std::string response;  
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());  
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, writeCallback);  
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);  
        curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5L);  

        CURLcode res = curl_easy_perform(curl.get());  
        if (res != CURLE_OK) {  
            throw std::runtime_error("API请求失败: " +   
                               std::string(curl_easy_strerror(res)));  
        }  

        simdjson::dom::parser parser;  
        simdjson::dom::element doc;  
        auto error = parser.parse(response).get(doc);  
        if (error) {  
            throw std::runtime_error("JSON解析失败");  
        }  

        std::string_view status;  
        error = doc["status"].get(status);  
        if (error || status != "1") {  
            return "";  
        }  

        simdjson::dom::array districts;  
        error = doc["districts"].get(districts);  
        if (error || districts.begin() == districts.end()) {  
            return "";  
        }  

        auto chinaDistrict = districts.at(0);  
        std::string_view name, level, adcode, polyline;  
    
        // 获取必要字段  
        if (chinaDistrict["name"].get(name) ||  
            chinaDistrict["level"].get(level) ||  
            chinaDistrict["adcode"].get(adcode) ||  
            chinaDistrict["polyline"].get(polyline)) {  
            return "";  
        }  

        SSDesc chinaDesc;
        chinaDesc.geoName = "中国";
        //chinaDesc.level = "country";  

         
        std::string polylineStr(polyline);  
        size_t start = 0;  
        size_t end = polylineStr.find('|');  

        while (start < polylineStr.length()) {  
            std::string polygonStr;  
            if (end != std::string::npos) {  
                polygonStr = polylineStr.substr(start, end - start);  
                start = end + 1;  
                end = polylineStr.find('|', start);  
            } else {  
                polygonStr = polylineStr.substr(start);  
                start = polylineStr.length();  
            }  

            std::vector<double> polygon;  
            size_t coordStart = 0;  
            size_t coordEnd = polygonStr.find(';');  

            while (coordStart < polygonStr.length()) {  
                std::string coordPair;  
                if (coordEnd != std::string::npos) {  
                    coordPair = polygonStr.substr(coordStart, coordEnd - coordStart);  
                    coordStart = coordEnd + 1;  
                    coordEnd = polygonStr.find(';', coordStart);  
                } else {  
                    coordPair = polygonStr.substr(coordStart);  
                    coordStart = polygonStr.length();  
                }  

                size_t commaPos = coordPair.find(',');  
                if (commaPos != std::string::npos) {  
                    try {  
                        double lng = std::stod(coordPair.substr(0, commaPos));  
                        double lat = std::stod(coordPair.substr(commaPos + 1));  
                        polygon.push_back(lng);  
                        polygon.push_back(lat);  
                    } catch (...) {  
                        continue;  
                    }  
                }  
            }  

            if (!polygon.empty()) {  
                // todo: 中国的边界要弄一下 
            }  
        }  

        ssDescs[chinaDesc.geoName] = std::move(chinaDesc);  
        return chinaDesc.geoName;  
    }


    bool getMinboundingRectangle(simdjson::dom::element &district, std::vector<GeoCoordinate> &geoPerimeter) {
         
       
    }

}

