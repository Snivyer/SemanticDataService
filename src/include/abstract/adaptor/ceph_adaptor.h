/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   ceph adaptor 
*/

#pragma once
#include "adaptor.h"


namespace SDS
{
    class CephAdaptor : public Adaptor
    {
    public:
        CephAdaptor(struct ConnectConfig &connConfig,  FilePathList *list);


        bool connect();

        size_t getSize();

        size_t getCapacity();

        std::string AllocateSpace(int spaceSize);

        // 从NC文件中读取变量信息
        bool getVarDescList(FilePathList pathList, std::vector<VarDesc> &descList);

        bool readVar(int nc_id, int groupID, std::string filePath);
        

        // 以数据箱子的方式将数据写入到存储系统中
        // virtual bool importFile();

        // 以NC文件的方式将数据写入存储系统中
        bool importFile(struct SystemDesc &sysDesc, FilePathList pathList);


    private:
        struct ConnectConfig connConfig;
        
    };


}
