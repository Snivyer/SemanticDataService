#include "abstract/adaptor/NC_pluge.h"

namespace SDS {


    bool NCPluge::open(std::string path, int &ncid) {
        if(nc_open(path.c_str(), NC_NOWRITE, &ncid) != NC_NOERR) {
            return false;
        }
        return true;
    }

    bool NCPluge::close(int ncid) {
        if(nc_close(ncid) != NC_NOERR) {
            return false;
        }
        return true;
    }

    void NCPluge::getGroupInfo(int ncid, int &gid, int &groupNum, std::string &groupName) {
        if (nc_inq_grps(ncid, &groupNum, &gid) != NC_NOERR) {
            close(ncid);
        }
        char groupNameStr[NC_MAX_NAME + 1];
        if (nc_inq_grpname(ncid, groupNameStr) != NC_NOERR) {
            close(ncid);
        }
        groupName = std::string(groupNameStr);
    }

    bool NCPluge::readVarDesc(int ncid, int varid, VarDesc &varDesc) {
        char varName[NC_MAX_NAME + 1];
        nc_type varType;
        int varNdims;
        int varDimids[NC_MAX_VAR_DIMS];
    
    
        if (nc_inq_var(ncid, varid, varName, &varType, &varNdims, varDimids, NULL) != NC_NOERR) {
            nc_close(ncid);
            return false;
        }
    
        varDesc.ncVarID = varid;
        varDesc.ncGroupID = -1;
        varDesc.varName = varName; 
    
        int total_size = 1;
        for (int dim = 0; dim < varNdims; dim++) {
            size_t dimLen;
            if (nc_inq_dimlen(ncid, varDimids[dim], &dimLen) != NC_NOERR) {
                nc_close(ncid);
                return false;
            }
            varDesc.shape.push_back(dimLen);
            total_size *= dimLen;
        }
    
        switch (varType) {
            case NC_BYTE:
                varDesc.varType = "NC_BYTE";
                varDesc.varLen = total_size;
                break;
            case NC_SHORT:
                varDesc.varType = "NC_SHORT";
                varDesc.varLen = total_size;
                break;
            case NC_INT:
                varDesc.varType = "NC_INT";
                varDesc.varLen = total_size;
                break;
            case NC_FLOAT:
                varDesc.varType = "NC_FLOAT";
                varDesc.varLen = total_size;
                break;
            case NC_DOUBLE:
                varDesc.varType = "NC_DOUBLE";
                varDesc.varLen = total_size;
                break;
            default:
                varDesc.varType = "NC_UNKNOWN";
        }
        return true;
    }

    bool NCPluge::readVarDesc(int ncid, int gid, int groupInd, int varid, VarDesc &varDesc) {
        char varName[NC_MAX_NAME + 1];
        nc_type varType;
        int varNdims;
        int varDimids[NC_MAX_VAR_DIMS];
 
        if (nc_inq_var(gid + groupInd, varid, varName, &varType, &varNdims, varDimids, NULL) != NC_NOERR) {
            close(ncid);
        }

        varDesc.ncVarID = varid;
        varDesc.ncGroupID = groupInd;
        varDesc.varName = varName; 

        int total_size = 1;
        for (int dim = 0; dim < varNdims; dim++) {
            size_t dimLen;
            if (nc_inq_dimlen(gid + groupInd, varDimids[dim], &dimLen) != NC_NOERR) {
                nc_close(ncid);
                return false;
            }
            varDesc.shape.push_back(dimLen);
            total_size *= dimLen;
        }

        switch (varType) {
            case NC_BYTE:
                varDesc.varType = "NC_BYTE";
                varDesc.varLen = total_size * sizeof(unsigned char);
                break;
            case NC_SHORT:
                varDesc.varType = "NC_SHORT";
                varDesc.varLen = total_size * sizeof(short);
                break;
            case NC_INT:
                varDesc.varType = "NC_INT";
                varDesc.varLen = total_size * sizeof(int);
                break;
            case NC_FLOAT:
                varDesc.varType = "NC_FLOAT";
                varDesc.varLen = total_size * sizeof(float);
                break;
            case NC_DOUBLE:
                varDesc.varType = "NC_DOUBLE";
                varDesc.varLen = total_size * sizeof(double);
                break;
            default:
                varDesc.varType = "NC_UNKNOWN";
        }
        return true;
    }

    bool NCPluge::readVarDescList(int ncid, std::vector<VarDesc> &descList) {
        int varNums;
        if (nc_inq_nvars(ncid, &varNums) != NC_NOERR) {
            nc_close(ncid);
            return false;
        }
    
        for (int varid = 1; varid < varNums; varid++) {
            VarDesc varDesc;
            if(readVarDesc(ncid, varid, varDesc)) {
                descList.emplace_back(varDesc);
            }        
        }
    }

    bool NCPluge::readVarDescList(int ncid, int gid, int groupNums, std::vector<VarDesc> &descList) {
        for(int gnum = 0; gnum < groupNums; gnum++) {
            int varNums;
            if (nc_inq_nvars(gid + gnum, &varNums) != NC_NOERR) {
                nc_close(ncid);
                return false;
            }
    
            for (int varid = 0; varid < varNums; varid++) {
                VarDesc varDesc;
                if(readVarDesc(ncid, gid, gnum, varid, varDesc)) {
                    descList.emplace_back(varDesc);
                }
            }       
        }
    }

    bool NCPluge::readVarList(std::string path, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray) {
        int ncid;
        int gid;
        if(nc_open(path.c_str(), NC_NOWRITE, &ncid) != NC_NOERR) {
            return false;
        }

        arrow::MemoryPool* pool = arrow::default_memory_pool();
        for(auto varDesc: descList) {
            if(varDesc.ncGroupID != -1) {
                // todo: 如果存在子分组，应该如何操作，是否需要一个从子组中读取数据的方法




            } else {
                int varid = varDesc.ncVarID;
                if(varDesc.varType == "NC_BYTE") {
                    unsigned char* data = new unsigned char(varDesc.varLen);
                    nc_get_var_ubyte(ncid, varDesc.ncVarID, data);
    
                    std::shared_ptr<arrow::Array> valueArray;
                    arrow::UInt8Builder builder(pool);
                    for(int i =0; i < varDesc.varLen; i++) {
                        builder.Append(*(data + i));
                    }
                    builder.Finish(&valueArray);
                    dataArray.push_back(valueArray);
                    delete data;
                } else if (varDesc.varType == "NC_SHORT") {
                    short* data = new short(varDesc.varLen);
                    nc_get_var_short(ncid, varDesc.ncVarID, data);
    
                    std::shared_ptr<arrow::Array> valueArray;
                    arrow::Int16Builder builder(pool);
                    for(int i =0; i < varDesc.varLen; i++) {
                        builder.Append(*(data + i));
                    }
                    builder.Finish(&valueArray);
                    dataArray.push_back(valueArray);
                    delete data;
                } else if (varDesc.varType == "NC_INT") {
                    int* data = new int(varDesc.varLen);
                    nc_get_var_int(ncid, varDesc.ncVarID, data);
    
                    std::shared_ptr<arrow::Array> valueArray;
                    arrow::Int32Builder builder(pool);
                    for(int i =0; i < varDesc.varLen; i++) {
                        builder.Append(*(data + i));
                    }
                    builder.Finish(&valueArray);
                    dataArray.push_back(valueArray);
                    delete data;
                    
                } else if (varDesc.varType == "NC_FLOAT") {
                    std::vector<float> data(varDesc.varLen);
                    nc_get_var_float(ncid, varDesc.ncVarID, data.data());
    
                    std::shared_ptr<arrow::Array> valueArray;
                    arrow::FloatBuilder builder(pool);
                    for(int i =0; i < varDesc.varLen; i++) {
                        builder.Append(data[i]);
                    }
                    builder.Finish(&valueArray);
                    dataArray.push_back(valueArray);
                } else if (varDesc.varType == "NC_DOUBLE") {
                    double* data = new double(varDesc.varLen);
                    nc_get_var_double(ncid, varDesc.ncVarID, data);
    
                    std::shared_ptr<arrow::Array> valueArray;
                    arrow::FloatBuilder builder(pool);
                    for(int i =0; i < varDesc.varLen; i++) {
                        builder.Append(*(data + i));
                    }
                    builder.Finish(&valueArray);
                    dataArray.push_back(valueArray);
                    delete data;
                } else {
                    nc_close(ncid);
                    return false;
                }
            }
        }
        nc_close(ncid);
        return true;
    }



}