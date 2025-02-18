#include "abstract/adaptor/HDF_pluge.h"

namespace SDS {
   
    H5::Group HDFPluge::open(std::string path) {
        const H5::H5File file(path, H5F_ACC_RDONLY);
        H5::Group group = file.openGroup("/");
        return group;
    }

    H5::Group HDFPluge::openGroup(H5::Group &group, std::string groupName) {
        return group.openGroup(groupName);
    }

    bool HDFPluge::readVarDesc(H5::DataSet &dataset, VarDesc &varDesc) {
        H5::DataType dtype = dataset.getDataType();
        H5::DataSpace dspace = dataset.getSpace();
        hsize_t size = dspace.getSimpleExtentNpoints();

        if (dtype == H5::PredType::NATIVE_INT) {
            varDesc.varType = "NATIVE_INT";
            varDesc.varLen = size;
        } else if (dtype == H5::PredType::NATIVE_FLOAT) {
            varDesc.varType = "NATIVE_FLOAT";
            varDesc.varLen = size;
        } else if (dtype == H5::PredType::NATIVE_DOUBLE) {
            varDesc.varType = "NATIVE_DOUBLE";
            varDesc.varLen = size;
        } else if (dtype == H5::PredType::NATIVE_CHAR) {
            varDesc.varType = "NATIVE_CHAR";
            varDesc.varLen = size;
        } else if (dtype == H5::PredType::NATIVE_UCHAR) {
            varDesc.varType = "NATIVE_UCHAR";
            varDesc.varLen = size;
        } else {
            varDesc.varType = "HDF_UNKNOWN";
            varDesc.varLen = size;
        }

    }

    bool HDFPluge::readVLDescList(H5::Group &group, std::string groupName, int groupID, VLDesc &vlDesc) {
        vlDesc.groupName = groupName;
        for (hsize_t i = 0; i < group.getNumObjs(); ++i) {
            H5std_string name = group.getObjnameByIdx(i);
            H5G_obj_t type = group.getObjTypeByIdx(i);
            if (type == H5G_GROUP) {
                H5::Group subgroup = group.openGroup(name);
                readVLDescList(subgroup, groupName + name, (int)i, vlDesc);
            } else if  (type == H5G_DATASET) {
                VarDesc varDesc;
                varDesc.varName = name;
                varDesc.ncVarID = i;
                varDesc.ncGroupID = groupID;
                H5::DataSet dataset = group.openDataSet(name);
                readVarDesc(dataset, varDesc);  
                vlDesc.desc.push_back(varDesc);
                vlDesc.varID.insert({name, vlDesc.desc.size()});
                vlDesc.groupLen += 1;
            }
        } 
    }


    bool HDFPluge::readVarList(std::string path, std::string groupName, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray) {
        H5::Group rootGroup = open(path);
        std::vector<std::string> names = splitString(groupName, '/');
        H5::Group group = rootGroup;

        for(auto name: names) {
            group = openGroup(group, name);
        }

        arrow::MemoryPool* pool = arrow::default_memory_pool();
        for(auto varDesc: descList) {
           H5::DataSet dataset = group.openDataSet(varDesc.varName);
           H5::DataType dtype = dataset.getDataType();
           int varid = varDesc.ncVarID;
           if (dtype == H5::PredType::NATIVE_INT) {
                int* data = new int(varDesc.varLen);
                dataset.read(data, dtype);
                std::shared_ptr<arrow::Array> valueArray;
                arrow::Int32Builder builder(pool);
                for(int i =0; i < varDesc.varLen; i++) {
                    builder.Append(*(data + i));
                }
                builder.Finish(&valueArray);
                dataArray.push_back(valueArray);
                delete data;
            } else if (dtype == H5::PredType::NATIVE_FLOAT) {
                float* data = new float(varDesc.varLen);
                dataset.read(data, dtype);
                std::shared_ptr<arrow::Array> valueArray;
                arrow::FloatBuilder builder(pool);
                for(int i =0; i < varDesc.varLen; i++) {
                    builder.Append(data[i]);
                }
                builder.Finish(&valueArray);
                dataArray.push_back(valueArray);
            } else if (dtype == H5::PredType::NATIVE_DOUBLE) {
                double* data = new double(varDesc.varLen);
                dataset.read(data, dtype);
                std::shared_ptr<arrow::Array> valueArray;
                arrow::FloatBuilder builder(pool);
                for(int i =0; i < varDesc.varLen; i++) {
                    builder.Append(*(data + i));
                }
                builder.Finish(&valueArray);
                dataArray.push_back(valueArray);
                delete data;
            } else if (dtype == H5::PredType::NATIVE_CHAR) {
                char* data = new char(varDesc.varLen);
                dataset.read(data, dtype);
                std::shared_ptr<arrow::Array> valueArray;
                arrow::Int16Builder builder(pool);
                for(int i =0; i < varDesc.varLen; i++) {
                    builder.Append(*(data + i));
                }
                builder.Finish(&valueArray);
                dataArray.push_back(valueArray);
                delete data;
            } else {
                continue;;
            }
        }
    }

}