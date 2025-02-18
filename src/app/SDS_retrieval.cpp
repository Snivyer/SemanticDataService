#include "app/SDS_retrieval.h"

namespace SDS_Retrieval {

    class SDS_Retrieval_Client::Impl {
        public:
            // semantic space cache
            std::unordered_map<std::string, SemanticSpace> semanticSpaceCache_;
            
            // storage space cache
            std::unordered_map<std::string, StorageSpace> storageSpaceCache_;

            // databox cache
            std::unordered_map<SDS::ContentID, DataboxObject*, ContentIDHasher> databoxsCache_;
        
            // meta service client
            std::shared_ptr< MetaServiceClient> meta_client_;
            
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
        std::cout << "欢迎使用语义数据服务检索系统，请输入相应指令：" << std::endl;
        std::cout << "创建空间:create/cr + 行政区划名 + 存储系统 + 根路径" << std::endl;
        std::cout << "加载空间:load/lo + 空间名" << std::endl;
        std::cout << "导入数据:import/im + 行政区划名 + 文件目录路径" << std::endl;
        std::cout << "查询数据:search/sea + 行政区划名 + 时间段 + 变量列表" << std::endl;
        std::cout << "展示元数据:show/sh + 元数据关键字(如:semanticspace或storespace等)" << std::endl;
        std::cout << "退出系统:quit/qu" << std::endl;
    }

    int SDS_Retrieval_Client::runServer() {
        std::string codes;
        std::vector<std::string> infos;

        while(true) {
            menu();
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
            bool isSuccess; 

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
            } else if ((op == "show") || (op == "sh")) {
                showContentInfo(infos);
            } else if ((op == "getdata") || (op == "ge")) {
                isSuccess = getData(infos);
            } else if ((op == "bind") || (op == "bi")) {
                isSuccess = createByBucket(infos);
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

        bool flag = true;
        if ((opType == "space") || (opType == "sp")) {
            // flag = searchDataBySSInfo(infos);
        } else if ((opType == "time") || (opType == "ti")) {
            // flag = searchDataByTimeInfo(infos);
        } else if ((opType == "var") || (opType == "va")) {
            // flag = searchDataByVarInfo(infos);
        } else if ((opType == "path") || (opType == "pa")) {
            // flag = searchDataByStorepath(infos);
        } else {
            if(infos.size() != 5) {
                std::cout << "指令不符合规范！" << std::endl;
                return false;
            }
           // flag = searchDataBySSTimeVarInfo(infos);
        } 
        return flag;
    }


    bool SDS_Retrieval_Client::importData(std::vector<std::string>& infos) {
        std::string ssName = infos[1];
        std::string target_path = infos[2];
        
        bool result;
        impl_->meta_client_->importDataFromLocal(ssName, ssName, target_path, result);
        return result;
    }

    bool SDS_Retrieval_Client::createByBucket(std::vector<std::string>& infos) {
        
    }


    void SDS_Retrieval_Client::showContentInfo(std::vector<std::string>& infos) {
        std::string opType = infos[1];
        transform(opType.begin(), opType.end(), opType.begin(), ::tolower);

        if ((opType == "semanticspace") || (opType == "sem")) {
            showSemanticSpace();
        } else if ((opType == "time") || (opType == "ti")) {
            // printTimeInfo();
        } else if ((opType == "storespace") || (opType == "st")) {
            showStorageSpace();
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

    std::vector<std::string> SDS_Retrieval_Client::parseSpaceInfo(std::string admin) {
        std::vector<std::string> result;
        int temp = 0;
        size_t SSId = 0;
        bool isNumber = true;

        for (auto& c : admin) {
            temp = (int)c;
            if (temp >= 48 && temp <= 57) {
                continue;
            }
            else {
                isNumber = false;
                break;
            }
        }
 
        if (isNumber) {
                SSId = std::atoi(admin.c_str());
                result.push_back(std::to_string(SSId));
        } else {
            result = splitString(admin, ',');
        }
        return result;
    }

    bool SDS_Retrieval_Client::createSemanticSpace(std::string SSName, std::vector<std::string> geoNames) {
        SemanticSpace space;
        auto ret = impl_->semanticSpaceCache_.find(SSName);
        if(ret != impl_->semanticSpaceCache_.end()) {
            return true;
        }
        
        auto status = impl_->meta_client_->createSemanticSpace(SSName, geoNames, space);
        if(status.ok()) {
            impl_->semanticSpaceCache_[SSName] = space;
            return true;
        }
        std::cout << "语义空间创建失败！" << std::endl;
        return false;
    }

    bool SDS_Retrieval_Client::loadSemanticSpace(std::string SSName) {
        SemanticSpace space;
        auto status = impl_->meta_client_->loadSemanticSpace(SSName, space);
        if(status.ok()) {
            impl_->semanticSpaceCache_[SSName] = space;
            return true;
        }
        std::cout << "语义空间加载失败！" << std::endl;
        return false;
    }

    void SDS_Retrieval_Client::showSemanticSpace() {

        if(impl_->semanticSpaceCache_.size() == 0) {
            std::cout << "暂时没有语义空间信息..." << std::endl;
        }

        for(auto space: impl_->semanticSpaceCache_) {
            space.second.print();
            std::cout << std::endl;
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
            return true;
        }
        std::cout << "存储空间创建失败！" << std::endl;
        return false;
    }

    void SDS_Retrieval_Client::showStorageSpace() {
        if(impl_->storageSpaceCache_.size() == 0) {
            std::cout << "暂时没有存储空间信息..." << std::endl;
        }

        for(auto space: impl_->storageSpaceCache_) {
            space.second.print();
            std::cout << std::endl;
        }
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

}