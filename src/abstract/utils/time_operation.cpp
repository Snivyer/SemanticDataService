#include "abstract/utils/time_operation.h"


namespace SDS {

bool string_to_tm(std::string timeStr, tm &time) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec =0;

    if(sscanf(timeStr.c_str(), "%4d%2d%2dT%2d%2d%2d", &year, &month, &day, &hour, &minute, &sec) != 6) {
        return false;
    } 

    memset(&time, 0, sizeof(tm));

    time.tm_year = year - 1900;
    time.tm_mon = month - 1;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = minute;
    time.tm_sec = sec;
    return true;
}

bool string_to_time(std::string timeStr, time_t &time) {
    tm tm1;
    memset(&tm1, 0, sizeof(tm1));
        
    if(string_to_tm(timeStr, tm1)) {
            time = mktime(&tm1);
    }
}

bool tm_to_string(struct tm &tm, std::string &timeStr) {
    char buffer[32];
    std::strftime(buffer, 32,  "%Y-%m-%d %H:%M:%S", &tm);
    timeStr = std::string(buffer);
}

bool compareTm(const tm& a, const tm &b) {

    if (a.tm_year != b.tm_year) {
        return a.tm_year < b.tm_year;
    } else if (a.tm_mon != b.tm_mon) {
        return a.tm_mon < b.tm_mon;
    } else if (a.tm_mday != b.tm_mday) {
        return a.tm_mday < b.tm_mday;
    } else if (a.tm_hour != b.tm_hour) {
        return a.tm_hour < b.tm_hour;
    } else if (a.tm_min != b.tm_min) {
        return a.tm_min < b.tm_min;
    } else {
        return a.tm_sec < b.tm_sec;
    }

}

}