#include "Daemon.h"

Daemon* Daemon::instance = nullptr; // definition class variable

// rules: multi-threaded work is always batched IO & multi-threaded work is mostly sequential anyway.

Daemon::Daemon() {
    unsigned int workerCt = m_processor_count - 1;

    if (workerCt == 0) {
        return;
    }

    daemon = std::thread(&Daemon::pollDaemon, this);
    busin.push_back(std::vector<std::tuple<std::tuple<short, short>, void*>>());
    busout.push_back(std::vector<std::tuple<std::tuple<short, short>, void*>>());

    unsigned int poolCt = workerCt - 1;

    for (unsigned int i = 0; i < poolCt; i++) {
        Workers.push_back(std::thread(&Daemon::pollWorker, this, i));
        busin.push_back(std::vector<std::tuple<std::tuple<short, short>, void*>>());
        busout.push_back(std::vector<std::tuple<std::tuple<short, short>, void*>>());
    }

}

Daemon::~Daemon() {
    threadStopped = true;
    daemon.join();
    for (int i = 0; i < Workers.size(); i++) {
        Workers[i].join();
    }
}

void Daemon::pollDaemon() {
	while (!threadStopped) {

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void Daemon::pollWorker(unsigned int workerCt) {
    unsigned int busIndex = workerCt + 1;
    while (!threadStopped) {

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}