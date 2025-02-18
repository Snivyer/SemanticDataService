#pragma once
#include "abstract/utils/string_operation.h"
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_meta_template.h"
#include <H5Cpp.h>
#include <arrow/api.h>

namespace SDS
{

    class HDFPluge  {
        public:
        
            H5::Group open(std::string path);

            H5::Group openGroup(H5::Group &group, std::string groupName);

          

            bool readVarDesc(H5::DataSet &dataset,  VarDesc &desc);

            bool readVLDescList(H5::Group &rootGroup, std::string groupName, int groupID, VLDesc &vlDes);

            bool readVarList(std::string path, std::string groupName, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray);

            
        
    };

}