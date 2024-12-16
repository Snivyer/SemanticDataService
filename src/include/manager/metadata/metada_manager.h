#pragma once

#include <unordered_map>
#include <unordered_set>
#include "manager/semanticspace/semantic_space.h"
#include "manager/storagespace/storage_space.h"


namespace SDS {

#define HANDLE_SIGPIPE(s, fd_)                                                      \
    do {                                                                                \
            Status _s = (s);                                                               \
            if (!_s.ok()) {                                                                \
               if (errno == EPIPE || errno == EBADF || errno == ECONNRESET) {            \
                    ARROW_LOG(WARNING)                                                   \
                         << "Received SIGPIPE, BAD FILE DESCRIPTOR, or ECONNRESET when " \
                         "sending a message to client on fd "                            \
                         << fd_ << ". "                                                  \
                         "The client on the other end may have hung up.";                \
                } else {                                                                  \
                    return _s;                                                           \
                }                                                                         \
            }                                                                              \
        } while (0);


    // communicate with metadata service
    struct MetaClient {
        int fd;

        MetaClient(int fd) {
          this->fd = fd;
        }

    };

    typedef struct {
        std::string semanticSpaceName;
        std::unordered_set<MetaClient*> clients;
        SemanticSpace* space; 
    } SemanticSpaceEntry;

    typedef struct {
        std::string storageSpaceName;
        std::unordered_set<MetaClient*> clients;
        StorageSpace* space; 
    } StorageSpaceEntry;



    struct MetaServiceInfo {
        std::unordered_map<std::string, SemanticSpaceEntry*> semanticSpaceInfo_;
        std::unordered_map<std::string, StorageSpaceEntry*> storageSpaceInfo_;
    };



}