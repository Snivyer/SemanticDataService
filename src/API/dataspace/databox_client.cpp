#include "API/dataspace/databox_client.h"


namespace SDS {

    class DataBoxClient::Impl {
        private:

            // File descriptor of the socket that connects to the store.
            int storeConn;
            // File descriptor of the socket that connects to the manager.
            int managerConn;
            // The amount of memory avaibale to the databox store
            int64_t storeCapacity;
            // A hash table of the databox IDs that are currently being used by this client
            std::unordered_map<ContentID, std::shared_ptr<DataboxInUseEntry>, ContentIDHasher> dbInUse;

            std::unordered_map<size_t, ContentID> dbIndex;

        public:
            Impl() {}
            ~Impl() {}
            void setStoreConn(int storeConn) {
                this->storeConn = storeConn;
            }

            int getStoreConn() {
                return this->storeConn;
            }

            void setManagerConn(int managerConn) {
                this->managerConn = managerConn;
            }

            int getManagerConn() {
                return this->managerConn;
            }

            int64_t getStoreCapacity() {
                return this->storeCapacity;
            }

            void setStoreCapacity(int64_t storeCapacity) {
                this->storeCapacity = storeCapacity;
            }

            std::unordered_map<ContentID, std::shared_ptr<DataboxInUseEntry>, ContentIDHasher>& getDBInUse() {
                return this->dbInUse;
            }

            std::unordered_map<size_t, ContentID> getDBIndex() {
                return this->dbIndex;
            }
            

    };

    DataBoxClient::DataBoxClient(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {} 
    DataBoxClient::~DataBoxClient() {}

    std::shared_ptr<DataBoxClient> DataBoxClient::createClient() {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>();
        std::shared_ptr<DataBoxClient> dbClient(new DataBoxClient(impl)); 
        return dbClient;
    }

    arrow::Status DataBoxClient::connect(const std::string& storeSocketName, 
                        const std::string& managerSocketName) {
 
    
        impl_->setStoreConn(connect_ipc_sock_retry(storeSocketName, -1, -1));
        if (managerSocketName != "") {
            impl_->setManagerConn( connect_ipc_sock_retry(managerSocketName, -1, -1));
        } else {
            impl_->setManagerConn(-1);
        }

        // send a ClientConnetRequest to the db store to get its memory capacity
        RETURN_NOT_OK(SendConnectRequest(impl_->getStoreConn()));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(impl_->getStoreConn(), MessageTypeConnectReply, &buffer));

        int64_t capacity;
        RETURN_NOT_OK(ReadConnectReply(buffer.data(), &capacity));
        impl_->setStoreCapacity(capacity);
        return Status::OK();
    }

    arrow::Status DataBoxClient::disconnect() {
        int fdConn = impl_->getStoreConn();
        close(fdConn);

        int fdMana = impl_->getManagerConn();
        if(fdMana >= 0) {
            close(fdMana);
        }

        return Status::OK();
    }


    arrow::Status DataBoxClient::createDB(ContentID &cntID, ContentDesc &cntDesc,
                                            StoreDesc &stoDesc, FilePathList &fileList, DBMeta& dbMeta) {
        int client = impl_->getStoreConn();
        RETURN_NOT_OK(SendCreateRequest(client, cntID, cntDesc, stoDesc, fileList));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeCreateReply, &buffer));
        RETURN_NOT_OK(ReadCreateReply(buffer.data(), dbMeta));

        if(dbMeta.varCount > 0) {
            dbMeta.filled = true;
        } 
        return Status::OK();                                      


    }

    arrow::Status DataBoxClient::getDB(ContentID &cntID, int64_t timeout, DataboxObject *object) {

        std::unordered_map<ContentID, std::shared_ptr<DataboxInUseEntry>, ContentIDHasher> useEntry = impl_->getDBInUse();
        auto dbEntry = useEntry.find(cntID);
        if(dbEntry != useEntry.end()) {
            ARROW_CHECK(dbEntry->second->isSeal) << "Client called get on an unsealed object that it created";
            
            // get databox object directly from local
            object = dbEntry->second->object;
            // increament the count of the number of instance of this object
            dbEntry->second->count += 1;   
            return Status::OK();
        }

        // get databox object from the databox store
        int client = impl_->getStoreConn();
        RETURN_NOT_OK(SendGetRequest(client, cntID, timeout));

        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeGetReply, &buffer));
        std::string ip;
        int port;
        RETURN_NOT_OK(ReadGetReply(buffer.data(), ip, port));

        if(port != 0) {
            ARROW_LOG(INFO) << "I start to prepare the arrow flight client at:" << ip <<":" << std::to_string(port);
            return Status::OK();
        } else {
            return Status::NotImplemented("There are not idel sender, try again!");
        }
    }

    arrow::Status DataBoxClient::getDB(size_t dbID, int64_t timeout, DataboxObject *object) {
        
        // std::unordered_map<ContentID, std::shared_ptr<DataboxInUseEntry>, ContentIDHasher> useEntry = impl_->getDBInUse();
        // auto dbEntry = useEntry.find(cntID);
        // if(dbEntry != useEntry.end()) {
        //     ARROW_CHECK(dbEntry->second->isSeal) << "Client called get on an unsealed object that it created";
            
        //     // get databox object directly from local
        //     object = dbEntry->second->object;
        //     // increament the count of the number of instance of this object
        //     dbEntry->second->count += 1;   
        //     return Status::OK();
        // }

        ContentID cntID;
        auto ret = this->getContentID(dbID, cntID);
        if(ret.ok()) {
            return this->getDB(cntID, timeout, object);
        } else {
            return Status::NotImplemented("There is not databox that you want, try again!");
        }
    }

    arrow::Status DataBoxClient::containDB(ContentID &cntID, bool &is_contain) {
        int client = impl_->getStoreConn();
    
        RETURN_NOT_OK(SendContainRequest(client, cntID));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeContaineReply, &buffer));

        RETURN_NOT_OK(ReadContainReply(buffer.data(), is_contain));
        return Status::OK();
    }

    arrow::Status DataBoxClient::releaseDB(ContentID &cntID, bool &is_release) {

        int client = impl_->getStoreConn();
        RETURN_NOT_OK(SendReleaseRequest(client, cntID));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeReleaseReply, &buffer));

        RETURN_NOT_OK(ReadReleaseReply(buffer.data(), is_release));
        return Status::OK();
    }

    arrow::Status DataBoxClient::deleteDB(ContentID &cntID, bool &is_delete) {
        int client = impl_->getStoreConn();
        RETURN_NOT_OK(SendDeleteRequest(client, cntID));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeDeleteReply, &buffer));

        RETURN_NOT_OK(ReadDeleteReply(buffer.data(), is_delete));
        return Status::OK();
    }

    arrow::Status DataBoxClient::getContentID(size_t dbID, ContentID &cntID) {
        auto ret = impl_->getDBIndex().find(dbID);
        if(ret != impl_->getDBIndex().end()) {
            cntID.setSpaceID(ret->second.getSpaceID());
            cntID.setTimeID(ret->second.getTimeID());
            cntID.setVarID(ret->second.getVarID());
            return Status::OK();
        }

        int client = impl_->getStoreConn();
        RETURN_NOT_OK(SendGetContentIDRequest(client, dbID));
        std::vector<uint8_t> buffer;
        RETURN_NOT_OK(messageReceive(client, MessageTypeGetContentIDReply, &buffer));
        RETURN_NOT_OK(ReadGetContentIDReply(buffer.data(), cntID));
        return Status::OK();

    }



}