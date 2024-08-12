/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   Local adaptor 
*/

#pragma once

#include "adaptor.h"
#include "third_party_lib/netcdf/include/netcdf.h"
#include "abstract/utils/string_operation.h"
#include "abstract/buffer/memory/memory.h"
#include <dirent.h>


namespace SDS
{
    class LocalAdaptor : public Adaptor
    {
    public:
        LocalAdaptor(struct ConnectConfig &connConfig);

        bool connect();

        size_t getSize();

        size_t getCapacity();

        std::string AllocateSpace(int spaceSize);

        // 从NC文件中读取变量信息
        bool getVarDescList(FilePathList pathList, std::vector<VarDesc> &descList, bool isSame = true);

        bool readVar(FilePathList &pathList, std::vector<VarDesc> &descList, STLBuffer &stlBuff);

        bool readVarList(std::string pathList, std::vector<VarDesc> &descList, STLBuffer &stlBuff);


        // 以数据箱子的方式将数据写入到存储系统中
        // virtual bool importFile();

        // 以NC文件的方式将数据写入存储系统中
        bool importFile(struct SystemDesc &sysDesc, FilePathList pathList);

    private:
        struct ConnectConfig connConfig;
        bool getVarDescInGroup(int ncid, int gid, int groupNum, std::vector<VarDesc> &descList);
        bool getVarDesc(int ncid, std::vector<VarDesc> &descList);
        bool getVarDesc(int ncid, int varid, VarDesc &varDesc);
        bool getVarDesc(int ncid, int gid, int gnum, int varid, VarDesc &varDesc);

    };


}
