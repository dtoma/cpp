#include <thread>
#include <uuid/uuid.h>

#include "lib.hpp"

using namespace dtoma;

int main() {
    #ifdef linux
	google_breakpad::MinidumpDescriptor descriptor("/tmp");
	google_breakpad::ExceptionHandler eh(descriptor, nullptr, dumpCallback, nullptr, true, -1);
	#endif

	log_queue_t log_q;
    std::thread logging_thread(get_log_reader(log_q));
    log_message(log_q, "Using zeromq v"s + get_zmq_version());

    zmq::context_t context(1);
    zmq::socket_t producer(context, ZMQ_REP);
	producer.bind("tcp://*:5555");
    log_message(log_q, "listening");

    while (true) {
        zmq::message_t request;
        producer.recv(&request);
		std::string message { static_cast<char*>(request.data()) };

		if (message.front() == 'R') { // Ready
			uuid_t msg_id;
			uuid_generate(msg_id);

			std::string msg_id_s(37, 0);
			uuid_unparse_lower(msg_id, &msg_id_s.front());
			log_message(log_q, "Send "s + msg_id_s);

			zmq::message_t reply(37);
			memcpy(reply.data(), msg_id_s.data(), 37);
			producer.send(reply);
		} else if (message.front() == 'D') { // Done
			log_message(log_q, "Result:"s + message.substr(1));
			zmq::message_t reply(0);
			producer.send(reply);
		}
    }

    logging_thread.join();
}
