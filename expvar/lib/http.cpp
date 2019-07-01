#include <unistd.h>
#include <sys/socket.h>

#include "http.h"

void dt::http::print_request(int pret, char* method, size_t method_len, char* path, size_t path_len,
                   int minor_version, size_t num_headers, phr_header* headers) {
    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", static_cast<int>(method_len), method);
    printf("path is %.*s\n", static_cast<int>(path_len), path);
    printf("HTTP version is 1.%d\n", minor_version);
    printf("headers:\n");
    for (size_t i = 0; i != num_headers; ++i) {
        printf("%.*s: %.*s\n", static_cast<int>(headers[i].name_len), headers[i].name,
               static_cast<int>(headers[i].value_len), headers[i].value);
    }
}

void dt::http::send_response(leveldb::DB* db, int new_fd) {
    std::string resp = "HTTP/1.0 200 OK\r\nfoo: \r\nfoo: b\r\n  \tc\r\n\r\n";
    std::string value;
    auto get = db->Get(leveldb::ReadOptions(), "Hello", &value);
    if (get.ok()) {
        resp += value + "\r\n";
    } else {
        resp += "No value found\n\r";
    }
    if (send(new_fd, resp.c_str(), resp.size(), 0) == -1)
        perror("send");
}

int dt::http::handle_request(leveldb::DB* db, int sockfd, int new_fd) {
    printf("Get rdy to parse request\n");
    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;
            
    while (1) {
        /* read the request */
        printf("Waiting for READ\n");
        while ((rret = read(new_fd, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR)
            ;
        if (rret <= 0) {
            return -1;
        }
        printf("READ OK\n");
        prevbuflen = buflen;
        buflen += rret;

        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen,
                                 const_cast<const char**>(&method), &method_len,
                                 const_cast<const char**>(&path), &path_len,
                                 &minor_version, headers, &num_headers, prevbuflen);
        if (pret > 0) {
            break; /* successfully parsed the request */
        }
        else {
            if (pret == -1) {
                printf("Error reading request\n");
                return -1;
            }
        }
        /* request is incomplete, continue the loop */
        assert(pret == -2);
        if (buflen == sizeof(buf)) {
            printf("Error #2\n");
            return -1;
        }
        printf("Request parsed OK\n");
    }

    dt::http::print_request(pret, method, method_len, path, path_len, minor_version, num_headers, headers);
    dt::http::send_response(db, new_fd);
    close(sockfd); // child doesn't need the listener
    close(new_fd);
    exit(0);
}
