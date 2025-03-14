#include "app/SDS_retrieval.h"

namespace SDS_Retrieval {

    class SDS_Retrieval_Client::Impl {
        public:

            // semantic space cache
            std::unordered_map<std::string, SemanticSpace> semanticSpaceCache_;
            std::unordered_map<std::string, SpaceInfo*> semanticSpaceTree_;
            
            // storage space cache
            std::unordered_map<std::string, StorageSpace> storageSpaceCache_;
            std::unordered_map<std::string, SpaceInfo*> storageSpaceTree_;

            // databox cache
            std::unordered_map<size_t, DataboxObject*> databoxsCache_;

            std::vector<FilePathList> resultCache_;
        
            // meta service client
            std::shared_ptr<MetaServiceClient> meta_client_;
            
            // databox client
            std::shared_ptr<DataBoxClient> db_client_;


            Impl() {}
            ~Impl() {}

    };

    SDS_Retrieval_Client::SDS_Retrieval_Client(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {

    }

    std::shared_ptr<SDS_Retrieval_Client> SDS_Retrieval_Client::createClient() {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>();
        std::shared_ptr<SDS_Retrieval_Client> client(new SDS_Retrieval_Client(impl)); 
        return client;
    }

    Status SDS_Retrieval_Client::connectMetaService(const std::string& metaSocketName, const std::string& ManagerSocketName) {
        impl_->meta_client_ = MetaServiceClient::createClient();
        Status ret = impl_->meta_client_->connect(metaSocketName, ManagerSocketName);
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "meta client connect success!  \n";
        }
        return ret;
    }

    Status SDS_Retrieval_Client::disconnectMetaService() {
        impl_->meta_client_->disconnect();
        return Status::OK();
    }

    Status SDS_Retrieval_Client::connectDBService(const std::string& dbSocketName, const std::string& ManagerSocketName) {
        impl_->db_client_ = DataBoxClient::createClient();
        Status ret = impl_->db_client_->connect(dbSocketName, ManagerSocketName);
        if(ret.ok()) {
            ARROW_LOG(INFO) <<  "databox client connect success!  \n";
        }
        return ret;
    }
    
    Status SDS_Retrieval_Client::disconnectDBService() {
        impl_->db_client_->disconnect();
        return Status::OK();
    }

    void SDS_Retrieval_Client::menu() { 
        std::cout << "创建空间:create/cr + 行政区划名 + 存储系统 + 根路径" << std::endl;
        std::cout << "加载空间:load/lo + 空间名" << std::endl;
        std::cout << "导入数据:import/im + 行政区划名 + 文件目录路径" << std::endl;
        std::cout << "查询数据文件:search/sea + 行政区划名 + 时间段 + 变量列表" << std::endl;
        std::cout << "查询数据箱子:find/fi + 行政区划名 + 时间段 + 变量列表" << std::endl;
        std::cout << "展示元数据:show/sh + 元数据关键字(如:semanticspace或storespace等)" << std::endl;
        std::cout << "详细展示元数据:detail/de +  元数据关键字(如:semanticspace或storespace等)" << std::endl;
        std::cout << "导出数据:export/ex + 导入数据类型(如file, databox等)" << std::endl; 
        std::cout << "帮助:help/he" << std::endl;
        std::cout << "退出系统:quit/qu" << std::endl;
    }

    int SDS_Retrieval_Client::runServer() {
        std::string codes;
        std::vector<std::string> infos;

        std::cout << "欢迎使用语义数据服务检索系统，请输入相应指令：" << std::endl;
        while(true) {
            if(codes != "") {
                std::cout << "请输入指令: " << std::endl;
            }
            std::getline(std::cin, codes);
            std::istringstream ss(codes);
            std::string code;
    
            while (std::getline(ss, code, ';')) 
            {
                std::istringstream codeStream(code);
                std::string singleInfo;
                while (codeStream >> singleInfo) {
                    infos.push_back(singleInfo);
                }
            }

            size_t size = infos.size();
            size_t inputInfoNum;
            bool isSuccess = true; 
            std::string op = infos[0];
            transform(op.begin(), op.end(), op.begin(), ::tolower);
            if((op == "import") || (op == "im")) {
                isSuccess = importData(infos);
            } else if ((op == "create") || (op == "cr")) {
                isSuccess = createSemanticStoreSpace(infos);
            } else if((op == "load") || (op == "lo")) {
                isSuccess = loadSemanticSpace(infos);
            } else if ((op == "search") || (op == "sea")) {
                isSuccess = searchData(infos);
            } else if ((op == "find") || (op == "fi")) {
                isSuccess = searchDataBox(infos);
            } else if ((op == "show") || (op == "sh")) {
                showContentInfo(infos);
            } else if ((op == "detail") || (op == "detail")) {
                detailContentInfo(infos);
            }
            else if ((op == "getdata") || (op == "ge")) {
                isSuccess = getData(infos);
            } else if ((op == "bind") || (op == "bi")) {
                isSuccess = createByBucket(infos);
            } else if (op == "export" || (op == "ex")) {
                isSuccess = exportData(infos);
            } else if (op == "help" || op == "he") {
                menu();
                isSuccess = true;
            } else if ((op == "quit") || (op == "qu")) {
                exitSys();
                std::cout << "成功退出系统！" << std::endl;
                break;
            } else {
                printOpTypeError(op);
                isSuccess = false;
            }
            printError(isSuccess, op);
            infos.clear(); 
        }
        return 0;
    }

    
    bool SDS_Retrieval_Client::searchData(std::vector<std::string>& infos) {
        std::string opType = infos[1];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);
        std::vector<std::string> times;
        std::vector<std::string> varNames;
        impl_->resultCache_.clear();

        if ((opType == "space")) {
            return searchDataFile(infos[2], times, varNames);
        } else if (opType == "time") {
            for(int i = 3; i < infos.size(); i++) {
                times.push_back(infos[i]);
            }
            return searchDataFile(infos[2], times, varNames);
        } else if (opType == "var") {
            for(int i = 3; i < infos.size(); i++) {
                varNames.push_back(infos[i]);
            }
            return searchDataFile(infos[2], times, varNames);
        } else if (opType == "all") {
            if(infos.size() > 5) {
                for(int i = 5; i < infos.size(); i++) {
                    varNames.push_back(infos[i]);
                }
                times.push_back(infos[3]);
                times.push_back(infos[4]);
            } else  {
                printOpTypeError(opType);
                return false;
            }
            return searchDataFile(infos[2], times, varNames);
        } else{
            printOpTypeError(opType);
            return false;
        }
    }

    bool SDS_Retrieval_Client::searchDataBox(std::vector<std::string>& infos) {
        std::string opType = infos[1];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);
        std::vector<std::string> times;
        std::vector<std::string> varNames;
        impl_->resultCache_.clear();

        if ((opType == "space")) {
            return searchDataBox(infos[2], times, varNames);
        } else if (opType == "time") {
            for(int i = 3; i < infos.size(); i++) {
                times.push_back(infos[i]);
            }
            return searchDataFile(infos[2], times, varNames);
        } else if (opType == "var") {
            for(int i = 3; i < infos.size(); i++) {
                varNames.push_back(infos[i]);
            }
            return searchDataFile(infos[2], times, varNames);
        } else if (opType == "all") {
            if(infos.size() > 5) {
                for(int i = 5; i < infos.size(); i++) {
                    varNames.push_back(infos[i]);
                }
                times.push_back(infos[3]);
                times.push_back(infos[4]);
            } else  {
                printOpTypeError(opType);
                return false;
            }
            return searchDataBox(infos[2], times, varNames);
        } else{
            printOpTypeError(opType);
            return false;
        }
    }


    bool SDS_Retrieval_Client::importData(std::vector<std::string>& infos) {
        std::string ssName = infos[1];
        std::string target_path = infos[2];
        bool result;
        impl_->meta_client_->importDataFromLocal(ssName, ssName, target_path, result);
        return result;
    }

    bool SDS_Retrieval_Client::exportData(std::vector<std::string>& infos) {
        
        std::string opType = infos[1];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);
        
    
        if ((opType == "file")) {
            std::string destPath = infos[2];
            return exportFile(destPath);
        } else if ((opType == "databox")) {
            size_t dbID = std::stoi(infos[2]);
            return exportDataBox(dbID);
        } else {
            printOpTypeError(opType);
            return false;
        }
    }


    bool SDS_Retrieval_Client::createByBucket(std::vector<std::string>& infos) {
        
    }


    void SDS_Retrieval_Client::showContentInfo(std::vector<std::string>& infos) {
        std::string opType = infos[1];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);

        if ((opType == "semanticspace") || (opType == "sem")) {
            if(infos.size() == 3) {
                showSemanticSpace(infos[2]);
            } else {
                showSemanticSpace();
            }
        } else if ((opType == "time") || (opType == "ti")) {

        } else if ((opType == "storespace") || (opType == "st")) {
            if(infos.size() == 3) {
                showStorageSpace(infos[2]);
            } else {
                showStorageSpace();
            }
           
        } else if ((opType == "groupbyvar") || (opType == "gr")) {
            // printVarGroupInfo();
        } else if ((opType == "indexbyvar") || (opType == "in")) {
            // printVarIndexsInfo();
        } else if ((opType == "databox") || (opType == "da")) {
            showDBInfo();
        } else {
            std::cout << "要展示的信息输入有误！" << std::endl;
        }
    }


    void SDS_Retrieval_Client::detailContentInfo(std::vector<std::string>& infos) {
        std::string opType = infos[1];
        std::string spaceName = infos[2];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);

        if ((opType == "semanticspace") || (opType == "sem")) {
            if(infos.size() == 4) {
                detailSemanticSpace(spaceName, infos[3]);
            } else {
                detailSemanticSpace(spaceName);
            }
        } else if ((opType == "time") || (opType == "ti")) {
            // printTimeInfo();
        } else if ((opType == "storespace") || (opType == "st")) {
            if(infos.size() == 4) {
                detailStorageSpace(spaceName, infos[3]);
            } else {
                detailStorageSpace(spaceName);
            }
        } else if ((opType == "groupbyvar") || (opType == "gr")) {
            // printVarGroupInfo();
        } else if ((opType == "indexbyvar") || (opType == "in")) {
            // printVarIndexsInfo();
        } else if ((opType == "databox") || (opType == "da")) {
            showDBInfo();
        } else {
            std::cout << "要展示的信息输入有误！" << std::endl;
        }
    }

    bool SDS_Retrieval_Client::createSemanticStoreSpace(std::vector<std::string>& infos) {
        std::string admin = infos[1];
        std::string spaceID;
        std::vector<std::string> geoNames = splitString(admin, '-');
        std::string ssName = geoNames[geoNames.size() - 1];

        // create semantic space and storage space
        bool ret = createSemanticSpace(ssName, geoNames);
        if(!ret) {
            return false;
        }
       
        // create storage space
        StoreTemplate temp;
        temp.SSName = ssName;
        temp.setStoreKind(infos[2]);
        temp.setPath(infos[3]);
        std::string storageID;
                
        ret = createStorageSpace(spaceID, ssName, temp);
        return ret;
    }

    bool SDS_Retrieval_Client::loadSemanticSpace(std::vector<std::string>& infos) {
        std::string ssName = infos[1];
        return loadSemanticSpace(ssName);
    }

    int SDS_Retrieval_Client::printError(bool flag, std::string code) {
        if(flag) {
            std::cout << "指令 \"" << code << "\" 执行成功！" << std::endl;
            std::cout << std::endl;
            return 0;
        }
        std::cout << "指令 \"" << code << "\" 执行失败！" << std::endl;
        std::cout << std::endl;
        return -1;
    }

    void SDS_Retrieval_Client::printOpTypeError(std::string op) {
        std::cout << "操作类型 \"" << op << "\" 不符合规范！" << std::endl;
    }

    void SDS_Retrieval_Client::exitSys() {
        // todo: 补充退出系统需要进行的操作
        std::cout << "正在退出系统..." << std::endl;
    }


    bool SDS_Retrieval_Client::createSemanticSpace(std::string SSName, std::vector<std::string> geoNames) {
        SemanticSpace space;
        auto ret = impl_->semanticSpaceCache_.find(SSName);
        if(ret != impl_->semanticSpaceCache_.end()) {
            return true;
        }
        auto status = impl_->meta_client_->createSemanticSpace(SSName, geoNames, space);
        if(status.ok()) {
            cacheSemanticSpace(space);
            return true;
        }
        std::cout << "语义空间创建失败！" << std::endl;
        return false;
    }

    bool SDS_Retrieval_Client::loadSemanticSpace(std::string SSName) {
        SemanticSpace space;
        auto status = impl_->meta_client_->loadSemanticSpace(SSName, space);
        if(status.ok()) {
            if(space.SSName == "") {
                std::cout << "不存在指定语义空间，请提前创建..." << std::endl;
                return false;
            } else {
                cacheSemanticSpace(space);
                return true;
            }
        }
        std::cout << "语义空间加载失败！" << std::endl;
        return false;
    }


    void SDS_Retrieval_Client::cacheSemanticSpace(SemanticSpace &space) {
        // insert space cahce
        std::string SSName = space.SSName;
        impl_->semanticSpaceCache_[SSName] = space;

        // insert space tree
        addToSemanticSpaceTree(space);
    }

    bool SDS_Retrieval_Client::addToSemanticSpaceTree(std::string spaceID, SpaceInfo* info, int keyLength) {
        if(spaceID.size() == 0) {
            return true;
        }

        auto ret = impl_->semanticSpaceTree_.find(spaceID);
        if(ret != impl_->semanticSpaceTree_.end()) {
            ret->second->children.insert({info->spaceID, info});
            info->parent = ret->second;
            return true;
        } else {
            SpaceInfo *pInfo = new SpaceInfo();
            pInfo->spaceName = "";
            pInfo->parent = nullptr;
            pInfo->spaceID = spaceID;
            pInfo->children.insert({info->spaceID, info});
            impl_->semanticSpaceTree_.insert({spaceID, pInfo});
            info->parent = pInfo;

            std::string PSSID = spaceID.substr(0, spaceID.size() - keyLength);
            return addToSemanticSpaceTree(PSSID, pInfo);
        }
    }

    bool SDS_Retrieval_Client::addToSemanticSpaceTree(SemanticSpace &space) {
        std::string spaceID = space.getCompleteSpaceID();
        auto ret = impl_->semanticSpaceTree_.find(spaceID);
        if(ret != impl_->semanticSpaceTree_.end()) {
            ret->second->spaceName = space.SSName;
            return addToSemanticSpaceTree(space.PSSID, ret->second);
        } else {
            SpaceInfo *info = new SpaceInfo();
            info->spaceName = space.SSName;
            info->parent = nullptr;
            info->spaceID = spaceID;
            impl_->semanticSpaceTree_.insert({spaceID, info});  
            return addToSemanticSpaceTree(space.PSSID, info);
        }
    }

    void SDS_Retrieval_Client::showSemanticSpace(std::string SSName) {
        if(SSName == "*") {
            if(impl_->semanticSpaceCache_.size() == 0) {
                std::cout << "暂时没有语义空间信息..." << std::endl;
            } else {
               printSemanticSpaceWithTreeView();
            }
        } else {
            auto ret = impl_->semanticSpaceCache_.find(SSName);
            if(ret != impl_->semanticSpaceCache_.end()) {
                printSemanticSpaceWithTreeView(ret->second.getCompleteSpaceID());
            } else {
                std::cout << "暂时没有创建该语义, 请预先创建..." << std::endl;
            }   
        }
    } 

    void SDS_Retrieval_Client::detailSemanticSpace(std::string SSName, std::string model) {
        SemanticSpace space;
        auto status = impl_->meta_client_->loadSemanticSpace(SSName, space);
        if(status.ok()) {
            impl_->semanticSpaceCache_[SSName] = space;
            if(model == "tree") {
                space.printWithTreeModel();
            } else {
                space.print();
            }

        } else {
            std::cout << "暂时没有创建该语义空间, 请预先创建..." << std::endl;
        }
    }

    bool SDS_Retrieval_Client::createStorageSpace(std::string spaceID, std::string SSName, StoreTemplate &temp) {
        StorageSpace space;
        auto ret = impl_->storageSpaceCache_.find(SSName);
        if(ret != impl_->storageSpaceCache_.end()) {
            return true;
        }
        
        auto status = impl_->meta_client_->createStorageSpace(spaceID, SSName, temp, space);
        if(status.ok()) {
            impl_->storageSpaceCache_[SSName] = space;
            cacheStorageSpace(space);
            return true;
        }
        std::cout << "存储空间创建失败！" << std::endl;
        return false;
    }

    bool SDS_Retrieval_Client::loadStorageSpace(std::string SSName) {
        StorageSpace space;
        auto status = impl_->meta_client_->loadStorageSpace(SSName, space);
        if(status.ok()) {
            if(space.stoMeta.SSName == "") {
                std::cout << "不存在指定存储空间，请提前创建..." << std::endl;
                return false;
            } else {
                cacheStorageSpace(space);
                return true;
            }
        }
        std::cout << "语义空间加载失败！" << std::endl;
        return false;
    }

    void SDS_Retrieval_Client::cacheStorageSpace(StorageSpace &space) {
        // insert space cahce
        std::string SSName = space.stoMeta.SSName;
        impl_->storageSpaceCache_[SSName] = space;

        // insert space tree
        addToStorageSpaceTree(space);
    }

    bool SDS_Retrieval_Client::addToStorageSpaceTree(std::string spaceID, SpaceInfo* info, int keyLength) {
        if(spaceID.size() == 0) {
            return true;
        }

        auto ret = impl_->storageSpaceTree_.find(spaceID);
        if(ret != impl_->storageSpaceTree_.end()) {
            ret->second->children.insert({info->spaceID, info});
            info->parent = ret->second;
            return true;
        } else {
            SpaceInfo *pInfo = new SpaceInfo();
            pInfo->spaceName = "";
            pInfo->parent = nullptr;
            pInfo->spaceID = spaceID;
            pInfo->children.insert({info->spaceID, info});
            impl_->storageSpaceTree_.insert({spaceID, pInfo});
            info->parent = pInfo;
            std::string PSSID = "";
            return addToStorageSpaceTree(PSSID, pInfo);
        }
    }

    bool SDS_Retrieval_Client::addToStorageSpaceTree(StorageSpace &space) {
        std::string spaceID = intToStringWithPadding(space.spaceID);
        auto ret = impl_->storageSpaceTree_.find(spaceID);
        if(ret != impl_->storageSpaceTree_.end()) {
            ret->second->spaceName = space.stoMeta.SSName;
            return addToStorageSpaceTree(0, ret->second);
        } else {
            SpaceInfo *info = new SpaceInfo();
            info->spaceName = space.stoMeta.SSName;
            info->parent = nullptr;
            info->spaceID = spaceID;
            impl_->storageSpaceTree_.insert({spaceID, info});  

            std::string pID = "0";
            return addToStorageSpaceTree(pID, info);
        }
    }

    void SDS_Retrieval_Client::showStorageSpace(std::string SSName) {
        if(SSName == "*") {
            if(impl_->storageSpaceCache_.size() == 0) {
                std::cout << "暂时没有存储空间信息..." << std::endl;
            } else {
               printStorageSpaceWithTreeView();
            }
        } else {
            auto ret = impl_->storageSpaceCache_.find(SSName);
            if(ret != impl_->storageSpaceCache_.end()) {
                printStorageSpaceWithTreeView(intToStringWithPadding( ret->second.spaceID));
            } else {
                std::cout << "暂时没有创建该存储空间, 请预先创建..." << std::endl;
            }   
        }
    } 

    void SDS_Retrieval_Client::detailStorageSpace(std::string SSName, std::string model) {
        StorageSpace space;
        auto status = impl_->meta_client_->loadStorageSpace(SSName, space);
        if(status.ok()) {
            impl_->storageSpaceCache_[SSName] = space;
            if(model == "tree") {
                space.printWithTreeModel();
            } else {
                space.print();
            }

        } else {
            std::cout << "暂时没有创建该存储空间, 请预先创建..." << std::endl;
        }
    }

    bool SDS_Retrieval_Client::searchDataFile(std::string SSName, std::vector<std::string> &times,
                                                 std::vector<std::string> &varNames) {
        auto status = impl_->meta_client_->searchDataFile(SSName, times, varNames, impl_->resultCache_);
        size_t spaceID = impl_->semanticSpaceCache_[SSName].spaceID;
        if(status.ok()) {
            return showSearchResult(SSName, std::to_string(spaceID));
        }
        std::cout << "暂时没有检索到该语义空间文件信息..." << std::endl;
        return true;
    }

    bool SDS_Retrieval_Client::searchDataBox(std::string SSName, std::vector<std::string> &times,
                                                 std::vector<std::string> &varNames) {

        std::vector<size_t> dbIDs;
        auto status = impl_->meta_client_->searchDataBox(SSName, times, varNames, impl_->resultCache_, dbIDs);
        if(status.ok()) {
            int totalFileNum = 0;
            for(auto pathList: impl_->resultCache_) {
                pathList.printWithTreeModel();
                totalFileNum += pathList.fileNames.size();
            }
            std::cout  << "本语义空间下共计检索出" << totalFileNum << "个文件" << std::endl;

            std::cout  << "本次预创建" << dbIDs.size() << "个数据箱子,其ID如下:" << std::endl;
            for(auto id : dbIDs) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
        }
        return true;

    }

    bool SDS_Retrieval_Client::showSearchResult(std::string spaceName, std::string spaceID) {
        int totalFileNum = 0;
        std::cout  << spaceName << " (" << spaceID << ")" << std::endl;
        for(auto pathList: impl_->resultCache_) {
            pathList.printWithTreeModel();
            totalFileNum += pathList.fileNames.size();
        }
        std::cout  << "本语义空间下共计检索出" << totalFileNum << "个文件" << std::endl;
        return true;
    }

    bool SDS_Retrieval_Client::getData(std::vector<std::string>& infos) {

    }
   
    bool SDS_Retrieval_Client::splitData(std::vector<std::string> &infos) {

    }

    bool SDS_Retrieval_Client::showDBInfo() {
        if(impl_->databoxsCache_.size() == 0) {
            std::cout << "暂时没有数据箱子..." << std::endl;
        }

        for(auto databox: impl_->databoxsCache_) {
            databox.second->print();
            std::cout << std::endl;
        }
    }

    bool SDS_Retrieval_Client::printSemanticSpaceWithTreeView(std::string spaceID) {
        auto ret = impl_->semanticSpaceTree_.find(spaceID);
        if(ret == impl_->semanticSpaceTree_.end()) {
            std::cout << "暂时没有语义空间信息..." << std::endl;
            return false;
        }
        
        printSemanticSpaceWithTreeView(ret->second, 0); 
        return true;
    }

    bool SDS_Retrieval_Client::printSemanticSpaceWithTreeView(SpaceInfo* info, int level) {
        if(info == nullptr) {
            return false;
        }
        for(int i = 0; i < level; i++) {
            std::cout << "  ";
        }

        if(info->spaceID == "001") {
            if(info->spaceName == "") {
                std::cout  << "缺省空间" << " (" << info->spaceID << ")" << std::endl;
            } else {
                std::cout  << info->spaceName << " (" << info->spaceID << ")" << std::endl;
            }
        } else {
            if(info->spaceName == "") {
                std::cout << "├─ " << "缺省空间" << " (" << info->spaceID << ")" << std::endl;
            } else {
                std::cout << "├─ " << info->spaceName << " (" << info->spaceID << ")" << std::endl;
            }
        }

        for(auto child: info->children) {
            printSemanticSpaceWithTreeView(child.second, level + 1);
        }
        return true;
    }

    bool SDS_Retrieval_Client::printStorageSpaceWithTreeView(std::string spaceID) {
        auto ret = impl_->storageSpaceTree_.find(spaceID);
        if(ret == impl_->storageSpaceTree_.end()) {
            std::cout << "暂时没有语义空间信息..." << std::endl;
            return false;
        }
        
        printStorageSpaceWithTreeView(ret->second, 0); 
        return true;

    }

    bool SDS_Retrieval_Client::printStorageSpaceWithTreeView(SpaceInfo* info, int level) {
        if(info == nullptr) {
            return false;
        }
        for(int i = 0; i < level; i++) {
            std::cout << "  ";
        }

        if(info->spaceID == "0") {
            if(info->spaceName == "") {
                std::cout  << "Local" << " (" << info->spaceID << ")" << std::endl;
            } else {
                std::cout  << info->spaceName << " (" << info->spaceID << ")" << std::endl;
            }
        } else {
            if(info->spaceName == "") {
                std::cout << "├─ " << "缺省空间" << " (" << info->spaceID << ")" << std::endl;
            } else {
                std::cout << "├─ " << info->spaceName << " (" << info->spaceID << ")" << std::endl;
            }
        }

        for(auto child: info->children) {
            printStorageSpaceWithTreeView(child.second, level + 1);
        }
        return true;
    }

    bool SDS_Retrieval_Client::exportFile(std::string destPath) {
        for(auto pathlist : impl_->resultCache_) {
            copyFiles(pathlist.getCompletePath(), pathlist.fileNames, destPath);
        }
        return true;
    }

    bool SDS_Retrieval_Client::exportDataBox(size_t dbID) {
        DataboxObject* dbObject;
        // todo: 这个对象是否也需要缓存下

        auto ret = impl_->db_client_->getDB(dbID, 0, dbObject);
        if(ret.ok()) {
            dbObject->print();
        }
        return true;
    }


}