#pragma once

#include <inttypes.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cassert>

#include <string>
#include <vector>

#include "arrow/status.h"
#include "arrow/util/logging.h"

namespace SDS {

#define PLASMA_PROTOCOL_VERSION 0x0000000000000000
#define DISCONNECT_CLIENT 0

    arrow::Status WriteBytes(int fd, uint8_t* cursor, size_t length);

    arrow::Status WriteMessage(int fd, int64_t type, int64_t length, uint8_t* bytes);

    arrow::Status ReadBytes(int fd, uint8_t* cursor, size_t length);

    arrow::Status ReadMessage(int fd, int64_t* type, std::vector<uint8_t>* buffer);

    int bind_ipc_sock(const std::string& pathname, bool shall_listen);

    int connect_ipc_sock(const std::string& pathname);

    int connect_ipc_sock_retry(const std::string& pathname, int num_retries, int64_t timeout);

    int AcceptClient(int socket_fd);

    uint8_t* read_message_async(int sock);
}

