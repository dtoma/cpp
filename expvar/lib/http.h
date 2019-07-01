#pragma once

#include "picohttpparser.h"
#include "leveldb/db.h"

namespace dt {
    namespace http {
        int handle_request(leveldb::DB* db, int sockfd, int new_fd);
        void send_response(leveldb::DB* db, int new_fd);
        void print_request(int pret, char* method, size_t method_len, char* path, size_t path_len,
                           int minor_version, size_t num_headers, phr_header* headers);
    }
}
