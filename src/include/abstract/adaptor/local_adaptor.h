/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   Local adaptor 
*/

#pragma once

#include "adaptor.h"
#include "abstract/utils/string_operation.h"
#include "abstract/utils/directory_operation.h"
#include <experimental/filesystem>


namespace SDS
{
    namespace fs = std::experimental::filesystem;
    class LocalAdaptor : public Adaptor
    {
    public:
        LocalAdaptor(ConnectConfig &connConfig,  FilePathList *list);
        bool connect();
        size_t getSize();
        size_t getCapacity();
        std::string AllocateSpace(int spaceSize);
        bool setFilePath(std::string path);
        

        // 从NC文件中读取变量信息
        bool getVarDescList(VLDesc &vlDesc, bool isSame = true);

        bool readVar(std::vector<VarDesc> &descList, std::vector<arrow::ArrayVector> &arrayVector2);

        bool readVarList(std::string filePath, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray);

        // 以数据箱子的方式将数据写入到存储系统中
        // virtual bool importFile();

        // 以NC文件的方式将数据写入存储系统中
        bool importFile(SystemDesc &sysDesc, FilePathList pathList);

    private:
        bool getVarDescListFromNC(std::string path, VLDesc &vlDesc);
        bool getVarDescListFromHDF(std::string path, VLDesc &vlDesc);

    };


}
