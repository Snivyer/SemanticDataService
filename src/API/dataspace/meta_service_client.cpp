#include "API/dataspace/meta_service_client.h"


namespace SDS {

    class MetaServiceClient::Impl {
        private:

            // File descriptor of the socket that connects to the meta services.
            int metaConn;

            // File descriptor of the socket that connects to the manager.
            int managerConn;

        public:
            Impl() {}
            ~Impl() {}
            void setMetaConn(int metaConn) {
                this->metaConn = metaConn;
            }

            int getMetaConn() {
                return this->metaConn;;
            }

            void setManagerConn(int managerConn) {
                this->managerConn = managerConn;
            }

            int getManagerConn() {
                return this->managerConn;
            }
    };

    MetaServiceClient::MetaServiceClient(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {} 
    MetaServiceClient::~MetaServiceClient() {}

    std::shared_ptr<MetaServiceClient>MetaServiceClient::createClient() {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>();
        std::shared_ptr<MetaServiceClient> client(new MetaServiceClient(impl)); 
        return client;
    }

    arrow::Status MetaServiceClient::connect(const std::string& storeSocketName, 
                        const std::string& managerSocketName) {
 
    
        impl_->setMetaConn(connect_ipc_sock_retry(storeSocketName, -1, -1));
        if (managerSocketName != "") {
            impl_->setManagerConn(connect_ipc_sock_retry(managerSocketName, -1, -1));
        } else {
            impl_->setManagerConn(-1);
        }

        // send a ClientConnetRequest to the meta services
        RETURN_NOT_OK(SendMetaConnectRequest(impl_->getMetaConn()));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(impl_->getMetaConn(), MessageTypeMetaConnectReply, &buffer));

        int64_t welcome;
        RETURN_NOT_OK(ReadConnectReply(buffer.data(), &welcome));
        return Status::OK();
    }

    arrow::Status MetaServiceClient::disconnect() {
        int fdConn = impl_->getMetaConn();
        close(fdConn);

        int fdMana = impl_->getManagerConn();
        if(fdMana >= 0) {
            close(fdMana);
        }

        return Status::OK();
    }

    arrow::Status MetaServiceClient::createSemanticSpace(std::string ssName, std::vector<std::string> &geoNames, std::string &spaceID) {
        int client = impl_->getMetaConn();
        RETURN_NOT_OK(SendCreateSemanticSpaceRequest(client, ssName, geoNames));
            
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeSemanticSpaceCreateReply, &buffer));
        RETURN_NOT_OK(ReadCreateSemanticSpaceReply(buffer.data(), spaceID));
        return Status::OK();      
    }

    arrow::Status MetaServiceClient::createStorageSpace(std::string spaceID, std::string ssName, StoreTemplate &temp, std::string &storageID) {
        
        std::string kindStr;
        if(temp.kind == StoreSpaceKind::Ceph) {
            kindStr = "Ceph";
        } else if(temp.kind == StoreSpaceKind::Lustre) {
            kindStr = "Lustre";
        } else if (temp.kind == StoreSpaceKind::BB) {
            kindStr = "BB";
        } else {
            kindStr = "None";
        }

        int client = impl_->getMetaConn();
        RETURN_NOT_OK(SendCreateStorageSpaceRequest(client,ssName, temp.spaceSize,
                                            spaceID, kindStr, temp.writable, temp.connConf));

        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeStorageSpaceCreateReply, &buffer));
        RETURN_NOT_OK(ReadCreateStorageSpaceReply(buffer.data(), storageID));
        return Status::OK();   
    }

    arrow::Status MetaServiceClient::importDataFromLocal(std::string semanticSpace, std::string storageSpace, std::string dirPath) {

        int client = impl_->getMetaConn();
        RETURN_NOT_OK(SendCreateContentIndexRequest(client, semanticSpace, storageSpace, dirPath));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeDataImportFromLocalReply, &buffer));

        int64_t welcome;
        RETURN_NOT_OK(ReadCreateContentIndexReply(buffer.data(), &welcome));
        return Status::OK();   
    }

    arrow::Status MetaServiceClient::searchContentIndex(std::vector<std::string> &geoNames,
                                                         std::vector<std::string> &times,
                                                         std::vector<std::string> &varNames,
                                                         ContentID &cntID,
                                                         std::string groupName) {

        int client = impl_->getMetaConn();
        RETURN_NOT_OK(SendSearchContentIndexRequest(client, geoNames, times, varNames, groupName));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeDataSearchReply, &buffer));

        std::string spaceID;
        std::string timeID;
        std::string varID;
        RETURN_NOT_OK(ReadSearchContentIndexReply(buffer.data(), spaceID, timeID, varID));
        cntID.setSpaceID(spaceID);
        cntID.setTimeID(timeID);
        cntID.setVarID(varID);
        return Status::OK();
    }

  

}