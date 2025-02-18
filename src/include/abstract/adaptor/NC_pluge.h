#pragma once
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_meta_template.h"
#include <netcdf.h>
#include <arrow/api.h>
using namespace arrow;

namespace SDS
{
    using namespace arrow;

    class NCPluge  {
        public:
            bool open(std::string path, int &ncid);
            bool close(int ncid);

            void getGroupInfo(int ncid, int &gid, int &groupNum, std::string &groupName);

            bool readVarDesc(int ncid, int varid, VarDesc &varDesc);

            bool readVarDesc(int ncid, int gid, int groupInd, int varID, VarDesc &varDesc);

            bool readVarDescList(int ncid, std::vector<VarDesc> &descList);

            bool readVarDescList(int ncid, int gid, int groupNums, std::vector<VarDesc> &descList);

            bool readVarList(std::string path, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray);
    };

}