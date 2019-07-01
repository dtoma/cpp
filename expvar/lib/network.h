#pragma once

#include <sys/socket.h>

#define PORT "4242"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

namespace dt {
    namespace net {
        void sigchld_handler(int);
        void* get_in_addr(struct sockaddr *sa);
        bool setup_dead_proc_catcher();
        int create_and_bind_socket();
        bool start_listening(int sockfd);
        int accept_client(int sockfd);
    }
}
