#include "utils/string_operation.h"


namespace SDS {

std::vector<std::string> splitString(const std::string &string, const char delimiter) {
    std::vector<std::string> tokens;
    char* token = strtok(const_cast<char*>(string.c_str()), &delimiter);
    while (token != nullptr) {
        tokens.emplace_back(token);
        token = strtok(nullptr, &delimiter);
    }
    return tokens;
}

std::string combinePath(const std::string &base_path, const std::string &sub_path) {
    std::string new_sub_path = sub_path;
    if (!sub_path.empty() && sub_path.front() == '/') {
        if (sub_path.size() == 1) {
            new_sub_path = "";
        } else {
            new_sub_path = sub_path.substr(1);
        }
    }
    if (base_path.empty() || base_path.back() == '/') {
        return base_path + new_sub_path;
    } else {
        return base_path + "/" + new_sub_path;
    }
}



}
