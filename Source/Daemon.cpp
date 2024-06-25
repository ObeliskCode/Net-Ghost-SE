#include "Daemon.h"

// rules: multi-threaded work is always batched IO & multi-threaded work is mostly sequential anyway.

Daemon::Daemon() {
    const auto workerCt = m_processor_count - 1;

    if (workerCt == 0) {
        return;
    }

    daemon = std::thread(&Daemon::pollDaemon);
    buses.push_back(std::vector<unsigned int, void*>());

    const auto poolCt = workerCt - 1;

    for (int i = 0; i < poolCt; i++) {
        Workers.push_back(std::thread(&Daemon::pollWorker));
        buses.push_back(std::vector<unsigned int, void*>());
    }

}

void Daemon::pollDaemon() {
	while (!threadStopped) {

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void Daemon::pollWorker() {
    while (!threadStopped) {

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
