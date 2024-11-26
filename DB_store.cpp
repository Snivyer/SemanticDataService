#include <vector>
#include <iostream>
#include <signal.h>
#include "API/dataspace/databox_store.h"
#include "abstract/adaptor/local_adaptor.h"
#include "manager/rpc/data_rpc/meta_server.h"
#include "manager/rpc/data_rpc/server.h"
using namespace std;
using namespace SDS;



    // Report "success" to valgrind.
    void signal_handler(int signal) {
        if (signal == SIGTERM) { exit(0); }
    }


    // start the databox store server
    void start_server(char* socketName, int64_t systemMemory, 
                std::shared_ptr<BasicMetaServer> rpcServer,
                std::vector<std::shared_ptr<BasicDataServer>> &senders ) {

        // Ignore SIGPIPE signals. If we don't do this, then when we attempt to write
        // to a client that has already died, the store could die.
        signal(SIGPIPE, SIG_IGN);

        ConnectConfig connConfig;
        connConfig.rootPath = "/";
        Adaptor *adatpor = new LocalAdaptor(connConfig);

        // Create the event loop.
        std::shared_ptr<EventLoop> loop(new EventLoop);
        std::shared_ptr<DataBoxStore> store =  DataBoxStore::createStore(loop, systemMemory, adatpor, rpcServer);

        for(auto sender : senders) {
            store->addDataServer(sender);
        }
    
        int socket = bind_ipc_sock(socketName, true);
        assert(socket >= 0);
        loop->add_file_event(socket, kEventLoopRead,
            [&store, socket](int events) { store->connectClient(socket); });
        loop->run();
    }   

    std::shared_ptr<BasicMetaServer> create_rpc_server(int port) {
        std::shared_ptr<BasicMetaServer> rpcServer = BasicMetaServer::Create();
        arrow::flight::Location bind_location;
        arrow::flight::Location::ForGrpcTcp("0.0.0.0", port).Value(&bind_location);

        arrow::flight::FlightServerOptions options(bind_location);            
        rpcServer->Init(options);
        rpcServer->SetShutdownOnSignals({SIGTERM});
        return rpcServer;
    }

    std::shared_ptr<BasicDataServer> create_rpc_data_server(std::string ip, int port, int serverPort)
    {
        std::shared_ptr<BasicDataServer> rpcDataServer = BasicDataServer::Create(ip);
        arrow::flight::Location bind_location;
        arrow::flight::Location::ForGrpcTcp(ip, port).Value(&bind_location);

        arrow::flight::FlightServerOptions options(bind_location);            
        rpcDataServer->Init(options);
        rpcDataServer->SetShutdownOnSignals({SIGTERM});
        rpcDataServer->Connect("0.0.0.0", serverPort);
        return rpcDataServer;
    }

    int main(int argc, char* argv[]) {
        signal(SIGTERM, signal_handler);
        char* socketName = NULL;
        int64_t port1 = 4444;
        int64_t port2 = 6666;
        int64_t port3 = 9999;
        int64_t systemMemory = -1;
        int c;
        while ((c = getopt(argc, argv, "s:m:p:c:")) != -1) {
            switch (c) {
            case 's':
                socketName = optarg;
                break;
            case 'm': {
                char extra;
                int scanned = sscanf(optarg, "%" SCNd64 "%c", &systemMemory, &extra);
                ARROW_CHECK(scanned == 1);
                ARROW_LOG(INFO) << "Allowing the Databox store to use up to "
                                << static_cast<double>(systemMemory) / 1000000000
                                << "GB of memory.";
                break;
            }
            case 'p':{
                char extra;
                int scanned = sscanf(optarg, "%" SCNd64 "%c", &port1, &extra);
                break;
            }
                case 'c':{
                char extra;
                int scanned = sscanf(optarg, "%" SCNd64 "%c", &port2, &extra);
                break;
            }
            default:
                exit(-1);
        }   
    }

    if (!socketName) {
        ARROW_LOG(FATAL) << "please specify socket for incoming connections with -s switch";
    }

    if (systemMemory == -1) {
        ARROW_LOG(FATAL) << "please specify the amount of system memory with -m switch";
    }

    ARROW_LOG(DEBUG) << "starting server listening on " << socketName;

    ARROW_LOG(INFO) << "rpc Metadata Server is running at " << std::to_string(port1);
    std::shared_ptr<BasicMetaServer> rpcServer = create_rpc_server(port1);
    std::thread rpcMetaThread([&rpcServer]() {
          rpcServer->Serve();
    });

    std::vector<std::shared_ptr<BasicDataServer>> senders;
    ARROW_LOG(INFO) << "rpc Data Server is running at " << std::to_string(port2);
    std::shared_ptr<BasicDataServer>  rpcDataServer1 = create_rpc_data_server("0.0.0.0", port2, port1);
    senders.push_back(rpcDataServer1);

    ARROW_LOG(INFO) << "rpc Data Server is running at " << std::to_string(port3);
    std::shared_ptr<BasicDataServer>  rpcDataServer2 = create_rpc_data_server("0.0.0.0", port3, port1);
    senders.push_back(rpcDataServer2);

    std::thread rpcDataThread1([&rpcDataServer1]() {
          rpcDataServer1->Serve();
    });

    std::thread rpcDataThread2([&rpcDataServer2]() {
          rpcDataServer2->Serve();
    });

    start_server(socketName, systemMemory, rpcServer, senders);
    rpcMetaThread.join();
    rpcDataThread1.join();
}