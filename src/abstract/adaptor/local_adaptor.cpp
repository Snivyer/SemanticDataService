#include "abstract/adaptor/local_adaptor.h"

namespace SDS {


    LocalAdaptor::LocalAdaptor(struct ConnectConfig &connConfig, FilePathList *list):Adaptor(connConfig, list) {
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


    bool LocalAdaptor::setFilePath(std::string path) {
        pathList->extractStoreSiteDesc(path);
        auto dirPath = combinePath(pathList->dirPath, pathList->sitePath);
        if(fs::exists(dirPath) && fs::is_directory(dirPath)) {
            for(auto& entry : fs::directory_iterator(dirPath)) {
                if(fs::is_regular_file(entry.status())){
                   pathList->fileNames.push_back(entry.path().filename());
                }
            }
            if(pathList->fileNames.size() > 0) {
                pathList->fileType = readFileExtension(pathList->fileNames[0]);
            }
            return true;
        }   
        return false;
    }


    bool LocalAdaptor::getVarDescList(VLDesc &vlDesc, bool isSame) {
        vlDesc.desc.clear();
        std::string dirPath = combinePath(pathList->dirPath, pathList->sitePath);
        for(std::string fileName : pathList->fileNames) {
            std::string path = combinePath(dirPath, fileName);
            std::string fileType = readFileExtension(path);
            if(fileType == "nc") {
                getVarDescListFromNC(path, vlDesc); 
            } else if (fileType == "HDF") { 
                getVarDescListFromHDF(path, vlDesc);
            } else { 
                return false;
            }
            if(isSame) {
                break;
            }
        }
        return true;
    }

    bool LocalAdaptor::readVarList(std::string path, std::vector<VarDesc> &descList, arrow::ArrayVector &dataArray) {
        std::string fileType = readFileExtension(path);
        if(fileType == "nc") {
            int ncid;
            auto pluge = PlugeFactory::getNCPluge();
            pluge->readVarList(path, descList, dataArray);
            delete pluge;
        } else if(fileType == "HDF") {
            auto pluge = PlugeFactory::getHDFPluge();
            std::string groupName;
            H5::Group group = pluge->readVarList(path, groupName, descList, dataArray);
        }
        return true;
       
    }

    bool LocalAdaptor::readVar(std::vector<VarDesc> &descList, std::vector<arrow::ArrayVector> &arrayVector2) {
        std::string dirPath = combinePath(pathList->dirPath, pathList->sitePath);
        for(std::string fileName : pathList->fileNames) {
            std::string path = combinePath(dirPath, fileName);
            std::string fileType = readFileExtension(path);
            arrow::ArrayVector dataArray;
            if(fileType == "nc") {
                int ncid;
                auto pluge = PlugeFactory::getNCPluge();
                pluge->readVarList(path, descList, dataArray);
                delete pluge;
            } else if(fileType == "HDF") {
                auto pluge = PlugeFactory::getHDFPluge();
                std::string groupName;
                H5::Group group = pluge->readVarList(path, groupName, descList, dataArray);
            }
            arrayVector2.push_back(dataArray);
        }
        return true;
    }

    std::string LocalAdaptor::AllocateSpace(int spaceSize) {

    }


    bool LocalAdaptor::importFile(struct SystemDesc &sysDesc, FilePathList pathList) {


    }

    bool LocalAdaptor::getVarDescListFromNC(std::string path, VLDesc &vlDesc) {
        // step1: open NetCDF文件
        int ncid;
        auto pluge = PlugeFactory::getNCPluge();
        if(!pluge->open(path, ncid) ) {
            return false;
        }

        // step2: get group info
        int gid;
        int groupNum = 0;
        std::string groupName;
        pluge->getGroupInfo(ncid, gid, groupNum, groupName);

        // step3: read varDesc
        if(groupNum == 0) {
            pluge->readVarDescList(ncid, vlDesc.desc);
        } else {
            pluge->readVarDescList(ncid, gid, groupNum, vlDesc.desc);
        }

        vlDesc.groupName = splitString(path, '.')[0];
        vlDesc.groupLen =  vlDesc.desc.size();
        for(int i = 0; i < vlDesc.groupLen; i++) {
            vlDesc.varID.insert({vlDesc.desc[i].varName, i});
        }

        pluge->close(ncid);
        delete pluge;
         
    }

    bool LocalAdaptor::getVarDescListFromHDF(std::string path, VLDesc &vlDesc) {
        // step1: open HDF5文件
        auto pluge = PlugeFactory::getHDFPluge();
        H5::Group rootGroup = pluge->open(path);

        // step2: read varDesc, defalut to exist many group in HDF5
        pluge->readVLDescList(rootGroup, "/", 0, vlDesc);

        vlDesc.groupName = splitString(path, '_')[4];
        vlDesc.groupLen = vlDesc.desc.size();
        pluge->readAttributes(rootGroup, vlDesc.attrs);
        vlDesc.attrLen = vlDesc.attrs.size();
        return true;
    }

}