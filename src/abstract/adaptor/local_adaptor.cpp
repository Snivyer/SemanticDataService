#include "abstract/adaptor/local_adaptor.h"

namespace SDS {


LocalAdaptor::LocalAdaptor(struct ConnectConfig &connConfig):Adaptor(connConfig) {
    connConfig = connConfig;
}

bool LocalAdaptor::connect() {
    std::string rootPath = connConfig.rootPath;
    if(fs::exists(rootPath) && fs::is_directory(rootPath)) {
        return true;
    }
    return false;
}

size_t LocalAdaptor::getSize() {
    return 4; 
}

size_t LocalAdaptor::getCapacity() {
    return 4; 
}

bool LocalAdaptor::getVarDesc(int ncid, int varid, VarDesc &varDesc) {
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

bool LocalAdaptor::getVarDesc(int ncid, int gid, int gnum, int varid, VarDesc &varDesc) {
    char varName[NC_MAX_NAME + 1];
    nc_type varType;
    int varNdims;
    int varDimids[NC_MAX_VAR_DIMS];

    if (nc_inq_var(gid + gnum, varid, varName, &varType, &varNdims, varDimids, NULL) != NC_NOERR) {
        nc_close(ncid);
        return false;
    }

    varDesc.ncVarID = varid;
    varDesc.ncGroupID = gnum;
    varDesc.varName = varName; 

    int total_size = 1;
    for (int dim = 0; dim < varNdims; dim++) {
        size_t dimLen;
        if (nc_inq_dimlen(gid + gnum, varDimids[dim], &dimLen) != NC_NOERR) {
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


bool LocalAdaptor::getVarDesc(int ncid, std::vector<VarDesc> &descList) {
    int varNums;
    if (nc_inq_nvars(ncid, &varNums) != NC_NOERR) {
        nc_close(ncid);
        return false;
    }

    for (int varid = 0; varid < varNums; varid++) {
        VarDesc varDesc;
        if(getVarDesc(ncid, varid, varDesc)) {
            descList.emplace_back(varDesc);
        }

        
    }
}


bool LocalAdaptor::getVarDescInGroup(int ncid, int gid, int groupNums, std::vector<VarDesc> &descList) {

    for(int gnum = 0; gnum < groupNums; gnum++) {
        int varNums;
        if (nc_inq_nvars(gid + gnum, &varNums) != NC_NOERR) {
            nc_close(ncid);
            return false;
        }

        for (int varid = 0; varid < varNums; varid++) {
            VarDesc varDesc;
            if(getVarDesc(ncid, gid, gnum, varid, varDesc)) {
                descList.emplace_back(varDesc);
            }

        }       
    }
}



bool LocalAdaptor::getFilePath(FilePathList &pathList, std::string dirPath) {

    pathList.dirPath = dirPath;
    if(fs::exists(dirPath) && fs::is_directory(dirPath)) {
        for(auto& entry : fs::directory_iterator(dirPath)) {
            if(fs::is_regular_file(entry.status())){
                pathList.fileNames.push_back(entry.path().filename());
            }
        }
        return true;
    }
    return false;
}


// fix: 如果列表中存在多种NC文件格式，那怎么办？
bool LocalAdaptor::getVarDescList(FilePathList pathList, std::vector<VarDesc> &descList, bool isSame) {

    descList.clear();
    std::string dirPath = combinePath(connConfig.rootPath, pathList.dirPath);

    for(std::string filepath : pathList.fileNames) {
   
        std::string path = combinePath(dirPath, filepath);
        int ncid;
        if(nc_open(path.c_str(), NC_NOWRITE, &ncid) != NC_NOERR) {
            return false;
        }

        int groupNums;
        int gid;
        if (nc_inq_grps(ncid, &groupNums, &gid) != NC_NOERR) {
            nc_close(ncid);
            return false;
        }

        if(groupNums == 0) {
            getVarDesc(ncid, descList);
        } else {
            getVarDescInGroup(ncid, gid, groupNums, descList);
        }

        if(isSame) {
            break;
        }
        nc_close(ncid);
    }
 
    return true;
}





bool LocalAdaptor::readVar(FilePathList &pathList, std::vector<VarDesc> &descList, STLBuffer &stlBuff) {

    std::string dirPath = combinePath(connConfig.rootPath, pathList.dirPath);
    for(std::string filepath : pathList.fileNames) {
        std::string path = combinePath(dirPath, filepath);
        readVarList(path, descList, stlBuff);
    }
    return true;
}


 bool LocalAdaptor::readVarList(std::string filePath, std::vector<VarDesc> &descList, STLBuffer &stlBuff) {

    int ncid;
    int gid;
    if(nc_open(filePath.c_str(), NC_NOWRITE, &ncid) != NC_NOERR) {
        return false;
    }

    int varListLen = 0;

    for(auto varDesc: descList) {
        varListLen += varDesc.varLen;
    }

    auto &buffer = stlBuff.buffer;
    auto &position = stlBuff.position;
    auto &absolutePosition = stlBuff.absolutedPosition;

    stlBuff.resize(position + varListLen);
    for(auto varDesc: descList) {
        if(varDesc.ncGroupID != -1) {
            // group方式
        } else {
            int varid = varDesc.ncVarID;
            if(varDesc.varType == "NC_BYTE") {
                unsigned char* data = new unsigned char(varDesc.varLen);
                nc_get_var_ubyte(ncid, varDesc.ncVarID, data);
                copyToBuffer<unsigned char>(buffer, position, data, size_t(varDesc.varLen));
                delete data;
            } else if (varDesc.varType == "NC_SHORT") {
                short* data = new short(varDesc.varLen);
                nc_get_var_short(ncid, varDesc.ncVarID, data);
                copyToBuffer<short>(buffer, position, data, varDesc.varLen);
                delete data;
            } else if (varDesc.varType == "NC_INT") {
                int* data = new int(varDesc.varLen);
                nc_get_var_int(ncid, varDesc.ncVarID, data);
                copyToBuffer<int>(buffer, position, data, varDesc.varLen);
                delete data;
            } else if (varDesc.varType == "NC_FLOAT") {
                float* data = new float(varDesc.varLen);
                nc_get_var_float(ncid, varDesc.ncVarID, data);
                copyToBuffer<float>(buffer, position, data, varDesc.varLen);
                delete data;
            } else if (varDesc.varType == "NC_DOUBLE") {
                double* data = new double(varDesc.varLen);
                nc_get_var_double(ncid, varDesc.ncVarID, data);
                copyToBuffer<double>(buffer, position, data, varDesc.varLen);
                delete data;
            } else {
                return false;
            }
        }
    }
    return true;
 }



std::string LocalAdaptor::AllocateSpace(int spaceSize) {

}


bool LocalAdaptor::importFile(struct SystemDesc &sysDesc, FilePathList pathList) {


}


}