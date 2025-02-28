
#pragma once

#include <vector>
#include <string>
#include <sys/time.h>
#include <ctime>
#include <cstring>
#include <iostream>

namespace SDS {
    bool string_to_tm(std::string timeStr, struct tm &time, std::string mode = "nc");       
    bool string_to_time(std::string timeStr, time_t &time, std::string mode = "nc"); 
    bool tm_to_string(struct tm &tm, std::string &timeStr, std::string mode = "nc");
    bool time_to_string(time_t &time, std::string &timeStr, std::string mode = "nc");
    bool compareTm(const tm& a, const tm &b);
}
 
