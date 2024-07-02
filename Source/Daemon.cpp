#include "Daemon.h"

Daemon* Daemon::instance = nullptr; // definition class variable

// rules: multi-threaded work is always batched IO & multi-threaded work is mostly sequential anyway.

Daemon::Daemon() {
    const auto workerCt = m_processor_count - 1;

    if (workerCt <= 0) {
        single_threaded = true;
        return;
    }

    daemon = std::thread(&Daemon::pollDaemon, this);
    op_in.push_back(std::vector<std::tuple<void* (*)(void*), void*>>());
    op_out.push_back(std::vector<std::tuple<short, void*>>());

    unsigned int poolCt = workerCt - 1;

    for (unsigned int i = 0; i < poolCt; i++) {
        Workers.push_back(std::thread(&Daemon::pollWorker, this, i));
        op_in.push_back(std::vector<std::tuple<void* (*)(void*), void*>>());
        op_out.push_back(std::vector<std::tuple<short, void*>>());
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

        data_in_mutex.lock();
        if (data_in.size() > 0){
            /* pop first element off stack*/
            const auto t = data_in[0];
            data_in.erase(0);
            data_in_mutex.unlock();
            
            /* do dispatch */
            OpFunc OF = std::get<0>(t);
            void* data = std::get<1>(t);
            // lock bus_in & bus_out completely
            OF.Dispatch(data);
            // unlock bus_in & bus_out completely
            
            /* do MT operations */

            /* Package Data to be receieved by recProcess */
            data_out_mutex.lock();
            
            data_out_mutex.unlock();
        } else {
            data_in_mutex.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void Daemon::pollWorker(unsigned int workerCt) {
    unsigned int busIndex = workerCt + 1;
    while (!threadStopped) {

        /* do MT operations */

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void* Daemon::blockingProcess(std::tuple<OpFunc, void*>){

}

unsigned short Daemon::sendProcess(std::tuple<OpFunc, void*>){

}

void* Daemon::recProcess(unsigned short ID){

}
