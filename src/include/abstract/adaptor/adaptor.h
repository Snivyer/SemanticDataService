/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
   storage adaptor used to adaptive to various file  systems, which inculde the basic operations
*/

#ifndef ADAPTOR_H_
#define ADAPTOR_H_

#include "manager/cnt_meta.h"
#include "manager/sto_meta.h"

namespace SDS
{
    class Adaptor
    {
    public:
        Adaptor(struct ConnectConfig &connConfig);
        ~Adaptor();


        virtual bool connect();
        virtual size_t getSize();
        virtual size_t getCapacity();
        virtual std::string AllocateSpace(int spaceSize);

        // 从NC文件中读取变量信息
        virtual bool getVarDescList(FilePathList pathList, std::vector<VarDesc> &descList);

        virtual bool readVar(int nc_id, int groupID, std::string filePath);
        

        // 以数据箱子的方式将数据写入到存储系统中
        // virtual bool importFile();

        // 以NC文件的方式将数据写入存储系统中
        virtual bool importFile(struct SystemDesc &sysDesc, FilePathList pathList);

        //

    private:
        struct ConnectConfig connConfig;

    };


}

#endif