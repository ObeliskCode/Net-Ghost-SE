#include "Daemon.h"

Daemon* Daemon::instance = nullptr; // definition class variable

// rules: multi-threaded work is always batched IO & multi-threaded work is mostly sequential anyway.

Daemon::Daemon() {
    const auto workerCt = m_processor_count - 1;

    if (workerCt <= 0) {
        single_threaded = true;
        return;
    }

    for (unsigned short i = 1; i <= SHORT_ID_MAX; i++) {
		availableIDs.push(i);
	}

    daemon = std::thread(&Daemon::pollDaemon, this);
    op_in.push_back(OP_IN());
    op_out.push_back(OP_OUT());

    unsigned int poolCt = workerCt - 1;

    for (unsigned int i = 0; i < poolCt; i++) {
        Workers.push_back(std::thread(&Daemon::pollWorker, this, i));
        op_in.push_back(OP_IN());
        op_out.push_back(OP_OUT());
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
            data_in.erase(data_in.begin());
            data_in_mutex.unlock();

            /* do dispatch */
            OpFunc OF = std::get<0>(t);
            void* data = std::get<1>(t);
            if (availableIDs.empty()) std::terminate();
            OF.PID = availableIDs.front();
            availableIDs.pop();

            for (int i = 0; i < op_in.size(); i++){
                op_in_mutex[i].lock();
            }

            OF.Dispatch(data,op_in,op_out);

            for (int i = 0; i < op_in.size(); i++){
                op_in_mutex[i].unlock();
            }

            /* do MT operations */

            /* Package Data to be receieved by recProcess */
            data_out_mutex.lock();
            void** dl;
            // aggregate data for package

            void* package = OF.Package(dl,OF.dataCt);
            data_out.push_back(DATA_OUT(OF.PID, package));
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

void* Daemon::blockingProcess(OpFunc of, void* data){
    if (single_threaded) return nullptr;
    // push datafunc tuple to data_in

    // somehow get PID of process

    // check for packaged results in data_out
    while (true) {

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

}
