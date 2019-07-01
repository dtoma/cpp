#include "network.h"

#include <sys/types.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

void dt::net::sigchld_handler(int) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void* dt::net::get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
    }
    return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
}

bool dt::net::setup_dead_proc_catcher() {
    struct sigaction sa;
    sa.sa_handler = dt::net::sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        perror("sigaction");
        return false;
    }
    return true;
}

int dt::net::create_and_bind_socket() {
    printf("Opening socket\n");

    struct addrinfo hints, *servinfo, *p;
    int rv;
    int yes=1;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(nullptr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this struct

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    return sockfd;
}

bool dt::net::start_listening(int sockfd) {
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        return false;
    }
    return true;
}

int dt::net::accept_client(int sockfd) {
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size = sizeof their_addr;
    int new_fd = accept(sockfd, reinterpret_cast<struct sockaddr *>(&their_addr), &sin_size);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }
    
    char s[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family,
              dt::net::get_in_addr(reinterpret_cast<struct sockaddr *>(&their_addr)),
              s, sizeof s);
    printf("server: got connection from %s\n", s);
    return new_fd;
}
