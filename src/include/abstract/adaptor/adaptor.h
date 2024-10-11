/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   storage adaptor used to adaptive to various file  systems, which inculde the basic operations
*/

#pragma once

#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_meta_template.h"
#include "abstract/buffer/malloc/STL_buffer.h"

namespace SDS
{
    class Adaptor
    {
    public:
        Adaptor(ConnectConfig &config):connConfig(config) {}




        virtual bool connect() {};
        virtual size_t getSize() {};
        virtual size_t getCapacity() {};
        virtual std::string AllocateSpace(int spaceSize) {};

        // get file path
        virtual bool getFilePath(FilePathList &pathList, std::string dirPath) {};

        // 从NC文件中读取变量信息
        virtual bool getVarDescList(FilePathList pathList, std::vector<VarDesc> &descList, bool isSame = true) {};

        virtual bool readVar(FilePathList &pathList, std::vector<VarDesc> &descList, STLBuffer &stlBuff) {};

        virtual bool readVarList(std::string pathList, std::vector<VarDesc> &descList, STLBuffer &stlBuff) {};


        // 以数据箱子的方式将数据写入到存储系统中
        // virtual bool importFile();

        // 以NC文件的方式将数据写入存储系统中
        virtual bool importFile(struct SystemDesc &sysDesc, FilePathList pathList) {};

        //

    private:
        struct ConnectConfig connConfig;

    };


}
