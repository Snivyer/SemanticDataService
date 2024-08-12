#include "abstract/utils/directory_operation.h"
#include <dirent.h>
#include <iostream>
using namespace std;

namespace SDS {

std::vector<std::string> getFileName(std::string dirName, bool subdir) {

    std::vector<std::string> fileNames; 
    DIR *dir = opendir(dirName.c_str());
    if (dir == nullptr) {
        return fileNames;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

            if(entry->d_type != DT_DIR) {
                fileNames.push_back(dirName + "/" + entry->d_name);
            }

            if(subdir == true &&  entry->d_type == DT_DIR) {
                std::vector<std::string> names = getFileName(dirName + "/" + entry->d_name);
                fileNames.insert(fileNames.begin(), names.begin(), names.end());
            }
        }
    }
    closedir(dir);
}

bool getFilePathList(std::string dirName, std::vector<FilePathList> &list, bool subdir) {

    DIR *dir = opendir(dirName.c_str());
    if (dir == nullptr) {
        return false;
    }

    FilePathList pathList;
    pathList.dirPath = dirName;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if(entry->d_type != DT_DIR) {
                pathList.fileNames.push_back(entry->d_name);
            } else {
                if(subdir == true) {
                   getFilePathList(dirName + "/" + entry->d_name, list);
                }
            }    
        }
    }
    closedir(dir);
    list.push_back(pathList);
    return true;
}




}