#include "SDSlet/SDSlet.h"


namespace SDSlet {


    class SDSlet::Impl {
        private:

            // basic configuration
            int64_t shareMemorySize_;
            std::string storeSocketName_;
            std::string metaSocketName_;
            std::shared_ptr<EventLoop> storeServiceLoop_;
            std::shared_ptr<EventLoop> metaServiceLoop_;
            
            int16_t maxReadNum_;
            int16_t maxSenderNum_;


            // service related to data databox reader
            std::shared_ptr<MetaService> metaService_;
            Adaptor* adaptor_;
            
            // service related to data dabox store
            std::shared_ptr<DataBoxStore> dbStore_;

            // service related to data box sender
            std::shared_ptr<BasicMetaServer> rpcServer_;
            std::vector<std::shared_ptr<BasicDataServer>> rpcDataServerPool_;

        public:
            Impl(int64_t shareMemorySize, std::string metaSocketName, std::string storeSocketName) {
                this->shareMemorySize_ = shareMemorySize;
                this->metaSocketName_ = metaSocketName;
                this->storeSocketName_ = storeSocketName;
                this->maxSenderNum_ = 2;
            }

            int64_t getShareMemorySize() {
                return this->shareMemorySize_;
            }

            std::string getMetaSocketName() {
                return this->metaSocketName_;
            }

            std::string getStoreSocketName() {
                return this->storeSocketName_;
            }

            int16_t getMaxSenderNum() {
                return this->maxSenderNum_;
            }

            void setStoreServiceEventLoop(std::shared_ptr<EventLoop> loop) {
                this->storeServiceLoop_ = loop;
            }

            std::shared_ptr<EventLoop> getStoreServiceEventLoop() {
                return this->storeServiceLoop_;
            }

            void setMetaServiceEventLoop(std::shared_ptr<EventLoop> loop) {
                this->metaServiceLoop_ = loop;
            }

            std::shared_ptr<EventLoop> getMetaServiceEventLoop() {
                return this->metaServiceLoop_;
            }

            void setAdaptor(Adaptor* adaptor) {
                this->adaptor_ = adaptor;
            }

            Adaptor* getAdaptor() {
                return this->adaptor_;
            }

            void setBasicMetaServer(std::shared_ptr<BasicMetaServer> rpcServer) {
                this->rpcServer_ = rpcServer;
            }

            std::shared_ptr<BasicMetaServer> getBasicMetaServer() {
                return this->rpcServer_;
            }

            void addBasicDataServer(std::shared_ptr<BasicDataServer> dataServer) {
                this->rpcDataServerPool_.push_back(dataServer);
            }

            std::vector<std::shared_ptr<BasicDataServer>>& getRpcDataServers() {
                return this->rpcDataServerPool_;
            }

            void setDBStore(std::shared_ptr<DataBoxStore> dbStore) {
                this->dbStore_ = dbStore;
            }

            std::shared_ptr<DataBoxStore> getDBStore() {
                return this->dbStore_;
            }

            void setMetaService(std::shared_ptr<MetaService> metaService) {
                this->metaService_ = metaService;
            }

            std::shared_ptr<MetaService> getMetaService() {
                return this->metaService_;
            }

    };

    std::shared_ptr<SDSlet> SDSlet::createSDSlet(int64_t shareMemorySize, std::string storeSocketName, std::string metaSocketName) {
        std::shared_ptr<Impl> impl = std::make_shared<Impl>(shareMemorySize, metaSocketName, storeSocketName);
        std::shared_ptr<SDSlet> sdslet(new SDSlet(impl)); 
        return sdslet;
    }

    Status SDSlet::init() {

        Status ret;
        // init the event loop 
        std::shared_ptr<EventLoop> storeServerloop(new EventLoop);
        std::shared_ptr<EventLoop> metaServerloop(new EventLoop);
        impl_->setStoreServiceEventLoop(storeServerloop);
        impl_->setMetaServiceEventLoop(metaServerloop);
        
        // init the metadata services
        ret =  createMetaServer();
        if(ret.ok()) {
            ARROW_LOG(INFO) << "Meta Server is running at " << impl_->getMetaSocketName();
        }


        ConnectConfig connConfig;
        FilePathList filePath;
        connConfig.rootPath = "/";
        Adaptor *adatpor = new LocalAdaptor(connConfig, &filePath);
        impl_->setAdaptor(adatpor);

        /* init the data box store service */ 
        // create the rpc metadata server
        int rpcServerPort = 4444;
        ret = createRpcServer(rpcServerPort);
        if(ret.ok()) {
            ARROW_LOG(INFO) << "rpc Metadata Server is running at " << std::to_string(rpcServerPort);
        }

        // create the rpc data server
        int startPort = 6666;
        std::string ip = "0.0.0.0";
        for(int16_t i = 0; i < impl_->getMaxSenderNum(); i++) {
            ret = createRpcDataServer(ip, startPort, rpcServerPort);
            if(ret.ok()) {
                ARROW_LOG(INFO) << "rpc Data Server is running at " << std::to_string(startPort);
            }
            startPort += 1;
        }

        // create the data box store
        ret = createDBStoreServer();
        if(ret.ok()) {
            ARROW_LOG(INFO) << "Data Store Service is running at " << impl_->getStoreSocketName();
        }

        return Status::OK();

    }

    Status SDSlet::run() {
        std::thread metaThread([this]() {
            this->impl_->getMetaServiceEventLoop()->run();
        });

      
        /* run the data box store service */ 
        // run the rpc metadata server thread
        std::shared_ptr<BasicMetaServer> rpcServer = impl_->getBasicMetaServer();
        std::thread rpcMetaThread([&rpcServer]() {
                    rpcServer->Serve();
        });
        

        // run the rpc data server thread
        std::vector<std::shared_ptr<BasicDataServer>> dataServerPool = impl_->getRpcDataServers();
        std::vector<std::thread> dataServerThreadPool;
        for(auto sender :  dataServerPool) {
            std::thread rpcDataThread([&sender]() {
                sender->Serve();
            });
            dataServerThreadPool.push_back(std::move(rpcDataThread));
        }

        std::thread dbStoreThread([this]() {
            this->impl_->getStoreServiceEventLoop()->run();
        });
       

        ARROW_LOG(INFO) << "SDSlet Services start...";
        metaThread.join();
        rpcMetaThread.join();
        dbStoreThread.join();

        for(int i = 0; i < dataServerThreadPool.size(); i++) {
            dataServerThreadPool[i].join();
        }
        return Status::OK();
    }



    SDSlet::~SDSlet() {

    }

    SDSlet::SDSlet(std::shared_ptr<Impl> impl):impl_(std::move(impl)) {

    }

    
    Status SDSlet::createRpcServer(int port) {
        std::shared_ptr<BasicMetaServer> rpcServer = BasicMetaServer::Create();
        arrow::flight::Location bind_location;
        arrow::flight::Location::ForGrpcTcp("0.0.0.0", port).Value(&bind_location);

        arrow::flight::FlightServerOptions options(bind_location);            
        rpcServer->Init(options);
        rpcServer->SetShutdownOnSignals({SIGTERM});
        impl_->setBasicMetaServer(rpcServer);
        return Status::OK();
    }

    Status SDSlet::createRpcDataServer(std::string ip, int port, int serverPort)
    {
        std::shared_ptr<BasicDataServer> rpcDataServer = BasicDataServer::Create(ip);
        arrow::flight::Location bind_location;
        arrow::flight::Location::ForGrpcTcp(ip, port).Value(&bind_location);

        arrow::flight::FlightServerOptions options(bind_location);            
        rpcDataServer->Init(options);
        rpcDataServer->SetShutdownOnSignals({SIGTERM});
        rpcDataServer->Connect("0.0.0.0", serverPort);
        impl_->addBasicDataServer(rpcDataServer);
        return Status::OK();
    }

    Status  SDSlet::createDBStoreServer() {
        std::shared_ptr<EventLoop> loop = impl_->getStoreServiceEventLoop();
        std::shared_ptr<BasicMetaServer> rpcServer = impl_->getBasicMetaServer();
        std::vector<std::shared_ptr<BasicDataServer>> dataServerPool = impl_->getRpcDataServers();
        int64_t shareMemorySize = impl_->getShareMemorySize(); 
        Adaptor* adaptor = impl_->getAdaptor();
        std::string socketName = impl_->getStoreSocketName();

        std::shared_ptr<DataBoxStore> store =  DataBoxStore::createStore(loop, shareMemorySize, adaptor, rpcServer);
        store->setMetaServer(impl_->getMetaService());

        impl_->setDBStore(store);
        for(auto sender :  dataServerPool) {
            store->addDataServer(sender);
        }

        int socket = bind_ipc_sock(socketName, true);
        assert(socket >= 0);
        auto storePtr = store.get();
        loop->add_file_event(socket, kEventLoopRead,
            [storePtr, socket](int events) { storePtr->connectClient(socket); });
       
        return Status::OK();
    }

    Status SDSlet::createMetaServer() {
        std::shared_ptr<EventLoop> loop = impl_->getMetaServiceEventLoop();
        std::shared_ptr<MetaService> metaService = MetaService::createMetaService(loop);
        impl_->setMetaService(metaService);
        std::string socketName = impl_->getMetaSocketName();
        int socket = bind_ipc_sock(socketName, true);
        assert(socket >= 0);

        auto metaServicePtr = metaService.get();
        loop->add_file_event(socket, kEventLoopRead,
            [metaServicePtr, socket](int events) { metaServicePtr->connectClient(socket); });
      
        return Status::OK();
    }

    










}