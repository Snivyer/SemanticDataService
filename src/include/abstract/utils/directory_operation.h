/* Author: Snivyer
*  Create on: 2024/7/26
*  Description:
   directory operation-realted functions
*/

#pragma once

#include <vector>
#include <string>
#include <cstring>
#include "manager/metadata/sto_meta.h"

namespace SDS {

    std::vector<std::string> getFileName(std::string dirName,bool subdir = true);
    bool getFilePathList(std::string dirName, std::vector<FilePathList> &list, bool subdir = true);

}

