/* Author: Snivyer
*  Create on: 2024/7/26
*  Description:
   directory operation-realted functions
*/

#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio> 
#include <dirent.h>   
#include <sys/stat.h>  
#include "manager/metadata/sto_meta.h"

namespace SDS {

   std::vector<std::string> getFileName(std::string dirName,bool subdir = true);
   bool getFilePathList(std::string dirName, std::vector<FilePathList> &list, bool subdir = true);
   std::string readFileExtension(std::string fileName);
   bool copyFile(const std::string& sourcePath, const std::string& destinationPath);
   bool copyFiles(const std::string& sourceDir, const std::vector<std::string>& filenames,
                    const std::string& destinationDir);
   bool hasFiles(const std::string& dirPath);
   void traverseDirectory(const std::string& rootPath, const std::string& dirPath, std::vector<std::string>& result);
   std::vector<std::string> getDirectoriesWithFiles(const std::string& rootPath);
    
}

