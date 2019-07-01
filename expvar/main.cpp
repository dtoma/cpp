#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <thread>

#include "http.h"
#include "network.h"
#include "db.h"

namespace {
    class expvar {
        std::thread t;

    public:
        template <typename T>
        expvar(T&& function) : t(function) {
        }

        ~expvar() {
            t.join();
        }

        expvar(expvar const&) = delete;
        expvar(expvar&&) = delete;

        expvar& operator=(expvar const&) = delete;
        expvar& operator=(expvar&&) = delete;
    };

    template <typename T>
    auto run_expvar_server(T* db) {
        auto sockfd = dt::net::create_and_bind_socket();
        auto listening = dt::net::start_listening(sockfd);
        if (!listening) {
            return 1;
        }

        auto ready = dt::net::setup_dead_proc_catcher();
        if (!ready) {
            return 1;
        }

        printf("socket waiting for connections\n");

        while(1) {  // main accept() loop
            auto new_fd = dt::net::accept_client(sockfd);
            if (new_fd == -1) {
                continue;
            }

            if (!fork()) { // this is the child process
                dt::http::handle_request(db, sockfd, new_fd);
            }
            close(new_fd);  // parent doesn't need this
        }
    }
}

int main() {
    auto db = dt::db::open_database();
    auto op = dt::db::put(db, "Hello", "World");
    if (!op.ok()) {
        return 1;
    }

    op = dt::db::put(db, "Foo", "Bar");
    op = dt::db::put(db, "Ping", "Pong");
    op = dt::db::put(db, "The Answer", "42");
    printf("Database dump:\n");
    dt::db::map(db, [](auto key, auto value) {
            printf("\t%-12s: %s\n", key.c_str(), value.c_str());
        });

    expvar e([&db]() { run_expvar_server(db); });
}
