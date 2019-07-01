#include <thread>

#include "lib.hpp"

using namespace dtoma;

int main(int ac, char** av) {
	if (ac < 2) {
		fmt::print("./client.exe [client id]");
		return 1;
	}
	std::string client_id { av[1] };
	
    #ifdef linux
	google_breakpad::MinidumpDescriptor descriptor("/tmp");
	google_breakpad::ExceptionHandler eh(descriptor, nullptr, dumpCallback, nullptr, true, -1);
	#endif

    log_queue_t log_q;
    std::thread logging_thread(get_log_reader(log_q));
    log_message(log_q, "Using zeromq v"s + get_zmq_version());
	
    zmq::context_t context(1);
    zmq::socket_t  worker(context, ZMQ_REQ);

    worker.connect("tcp://localhost:5555");

    while (true) {
        zmq::message_t ready(20);
        snprintf(static_cast<char*>(ready.data()), 20, "R %s", client_id.c_str());
        worker.send(ready);

		zmq::message_t task;
        worker.recv(&task);
		std::string payload { static_cast<char*>(task.data()) };
		log_message(log_q, "Received "s + payload);
		
        zmq::message_t reply(2); // snprintf null-terminates
        snprintf(static_cast<char*>(reply.data()), 2, "D");
        worker.send(reply);

		zmq::message_t ack;
		worker.recv(&ack); // to put the socket back in the right state

		sleep(1);
    }

    logging_thread.join();
}
