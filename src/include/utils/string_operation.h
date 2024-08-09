/* Author: Snivyer
*  Create on: 2024/3/27
*  Description:
   std::string operation-realted functions
*/

#pragma once

#include <vector>
#include <string>
#include <cstring>



namespace SDS {

    std::vector<std::string> splitString(const std::string &string, const char delimiter = '-');
    std::string combinePath(const std::string &base_path, const std::string &sub_path);

}
 
