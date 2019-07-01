#pragma once

#include "atomicops.h"
#include "fmt/format.h"
#include "readerwriterqueue.h"
#include "zmq.hpp"
#include <array>
#include <chrono>
#include <string>
#include <unistd.h>

using namespace std::string_literals;

namespace dtoma {

using clock_t = std::chrono::system_clock;

struct logItem {
    decltype(clock_t::now()) time_point = clock_t::now();
    std::string              message = "";
};

using log_queue_t = moodycamel::BlockingReaderWriterQueue<logItem>;

// Returns the version of zeromq as a string "major.minor.patch"
auto get_zmq_version() {
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);

    return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);
}

// Add a log_item to a given log queue
auto log_message(log_queue_t& q, std::string const& msg) {
    q.enqueue({ clock_t::now(), msg });
}

// Returns a function that unpiles logs from a given log queue
auto get_log_reader = [](log_queue_t& log_q) {
    return [&]() {
        logItem item;
        while (true) {
            log_q.wait_dequeue(item);

            auto const time_point = item.time_point;
            auto       time = clock_t::to_time_t(time_point);
            auto       tm = *std::gmtime(&time);

            auto epoch = time_point.time_since_epoch();
            auto us =
                std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % 1000000;

            std::array<char, 64> buf;
            std::strftime(buf.data(), buf.size(), "(%Z) %F %T", &tm);
			fmt::print("{}.{:06} | {}\n", buf.data(), us, item.message.c_str());
        }
    };
};

#ifdef linux
#include "client/linux/handler/exception_handler.h"
bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void*, bool succeeded) {
  printf("Dump path: %s\n", descriptor.path());
  return succeeded;
}
#endif

} // namespace dtoma
