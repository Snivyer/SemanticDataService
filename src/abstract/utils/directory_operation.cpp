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

    std::string readFileExtension(std::string fileName) {
        size_t pos = fileName.find_last_of(".");
        if (pos == std::string::npos) {
            return ""; 
        }
        return fileName.substr(pos + 1);
    }

    bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
        std::ifstream source(sourcePath, std::ios::binary);
        std::ofstream destination(destinationPath, std::ios::binary);
    
        if (!source.is_open()) {
            return false;
        }
    
        if (!destination.is_open()) {
            source.close();
            return false;
        }
    
        const size_t bufferSize = 8192;
        char buffer[bufferSize];
    
        while (source.read(buffer, bufferSize) || source.gcount() > 0) {
            destination.write(buffer, source.gcount());
        }
    
        source.close();
        destination.close();
        return true;
    }

    bool copyFiles(const std::string& sourceDir, const std::vector<std::string>& filenames, const std::string& destinationDir) {

        DIR *dir = opendir(destinationDir.c_str());
        if (dir == nullptr) {
            return false;
        }
        for (const std::string& filename : filenames) {
            std::string sourcePath = sourceDir + "/" + filename;
            std::string destinationPath = destinationDir + "/" + filename;
            copyFile(sourcePath, destinationPath);
        }
        return true;
    }

    bool hasFiles(const std::string& dirPath) {
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) {
            return false; 
        }
    
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string entryName = entry->d_name;
            if (entryName == "." || entryName == "..") {
                continue;
            }
    
            std::string fullPath = dirPath + "/" + entryName;
            struct stat statbuf;
            if (stat(fullPath.c_str(), &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
                closedir(dir);
                return true; 
            }
        }
    
        closedir(dir);
        return false; 
    }
    
    void traverseDirectory(const std::string& rootPath, const std::string& dirPath, std::vector<std::string>& result) {
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) {
            std::cout << "Error opening directory: " << dirPath << std::endl;
            return;
        }
    
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string entryName = entry->d_name;
            if (entryName == "." || entryName == "..") {
                continue;
            }
    
            std::string fullPath = dirPath + "/" + entryName;
            struct stat statbuf;
            if (stat(fullPath.c_str(), &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    traverseDirectory(rootPath, fullPath, result);
                    if (hasFiles(fullPath)) {
                        std::string relativePath = fullPath.substr(rootPath.length());
                        result.push_back(relativePath);
                    }
                }
            }
        }
        closedir(dir);
    }
    
    std::vector<std::string> getDirectoriesWithFiles(const std::string& rootPath) {
        std::vector<std::string> result;
        traverseDirectory(rootPath, rootPath, result);
        if (hasFiles(rootPath)) {
            result.push_back(rootPath);
        }
    
        std::sort(result.begin(), result.end());
        return result;
    }

}