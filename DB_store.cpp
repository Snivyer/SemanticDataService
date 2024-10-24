#include <vector>
#include <iostream>
#include <signal.h>
#include "manager/databox/databox_store.h"
using namespace std;
using namespace SDS;



    // Report "success" to valgrind.
    void signal_handler(int signal) {
        if (signal == SIGTERM) { exit(0); }
    }


    // start the databox store server
    void start_server(char* socketName, int64_t systemMemory) {

        // Ignore SIGPIPE signals. If we don't do this, then when we attempt to write
        // to a client that has already died, the store could die.
        signal(SIGPIPE, SIG_IGN);

        // Create the event loop.
        std::shared_ptr<EventLoop> loop(new EventLoop);
        std::shared_ptr<DataBoxStore> store =  DataBoxStore::createStore(loop, systemMemory);
    
        int socket = bind_ipc_sock(socketName, true);
        assert(socket >= 0);
        loop->add_file_event(socket, kEventLoopRead,
            [&store, socket](int events) { store->connectClient(socket); });
        loop->run();
    }   


    int main(int argc, char* argv[]) {

        signal(SIGTERM, signal_handler);
        char* socketName = NULL;
        int64_t systemMemory = -1;
        int c;
        while ((c = getopt(argc, argv, "s:m:")) != -1) {
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
    start_server(socketName, systemMemory);
}