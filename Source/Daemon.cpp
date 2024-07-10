#include "Daemon.h"

Daemon* Daemon::instance = nullptr; // definition class variable

// rules: multi-threaded work is always batched IO & multi-threaded work is mostly sequential anyway.

Daemon::Daemon() {
    const auto workerCt = m_processor_count - 1;
    std::cout << "workerCt " << workerCt << std::endl;

    if (workerCt <= 0) {
        single_threaded = true;
        return;
    }

    for (unsigned short i = 1; i <= SHORT_ID_MAX; i++) {
		availableIDs.push(i);
	}

    op_in_vec2d.push_back(OP_IN_VEC());
    op_out_vec2d.push_back(OP_OUT_VEC());

    unsigned int poolCt = workerCt - 1;

    for (unsigned int i = 0; i < poolCt; i++) {
        op_in_vec2d.push_back(OP_IN_VEC());
        op_out_vec2d.push_back(OP_OUT_VEC());
        Workers.push_back(std::thread(&Daemon::pollWorker, this, i));
    }
    daemon = std::thread(&Daemon::pollDaemon, this);

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
        if (data_in_vec.size() > 0){
            /* pop first element off stack*/
            const DATA_IN t = data_in_vec[0];
            data_in_vec.erase(data_in_vec.begin());
            data_in_mutex.unlock();

            /* do dispatch */
            OpFunc OF = std::get<0>(t);
            void* data = std::get<1>(t);

            for (int i = 0; i < op_in_vec2d.size(); i++){
                op_in_vec_mutex[i].lock();
            }

            // dispatch function could be templated out!
            OF.Dispatch(data, OF, op_in_vec2d);

            for (int i = 0; i < op_in_vec2d.size(); i++){
                op_in_vec_mutex[i].unlock();
            }

            /* do MT operations */
            // for parity reasons we lock op_in_mutex[0]
            op_in_vec_mutex[0].lock();

                for (auto it = op_in_vec2d[0].begin(); it != op_in_vec2d[0].end(); it++){
                    const OP_IN t = *it;
                    op_in_vec2d[0].erase(it);

                    OP operation = std::get<0>(t);
                    unsigned int idx = std::get<2>(t);
                    void* datum = std::get<3>(t);

                    void* result = operation(datum);

                    op_out_vec_mutex[0].lock();
                    op_out_vec2d[0].push_back(OP_OUT(std::get<1>(t), idx, result));
                    op_out_vec_mutex[0].unlock();
                }

            op_in_vec_mutex[0].unlock();

            /* Package Data to be receieved by recProcess */
            data_out_mutex.lock();
            void** dl = new void*[OF.dataCt];

            bool packaging = true;
            unsigned int packingCt = 0;

            while(packaging){
                for (int i = 0; i < op_out_vec2d.size(); i++){
                    op_out_vec_mutex[i].lock();
                }

                for (auto it = op_out_vec2d.begin(); it != op_out_vec2d.end(); it++){
                    OP_OUT_VEC ops = *it;
                    for (auto it = ops.begin(); it != ops.end(); it++){
                        OP_OUT opt = *it;
                        unsigned short pid = std::get<0>(opt);
                        if (pid == OF.PID){
                            unsigned int idx = std::get<1>(opt);
                            void* res = std::get<2>(opt);
                            dl[idx] = res;
                            ops.erase(it);
                        }
                    }
                }

                for (int i = 0; i < op_out_vec2d.size(); i++){
                    op_out_vec_mutex[i].lock();
                }

                if (packingCt == OF.dataCt) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }


            // TODO: change Package func API!
            void* package = OF.Package(dl,OF.dataCt);
            data_out_vec.push_back(DATA_OUT(OF.PID, package));

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
        op_in_vec_mutex[busIndex].lock();

            for (auto it = op_in_vec2d[busIndex].begin(); it != op_in_vec2d[busIndex].end(); it++){
                const OP_IN t = *it;
                op_in_vec2d[busIndex].erase(it);

                OP operation = std::get<0>(t);
                unsigned int idx = std::get<2>(t);
                void* datum = std::get<3>(t);

                void* result = operation(datum);

                op_out_vec_mutex[busIndex].lock();
                op_out_vec2d[busIndex].push_back(OP_OUT(std::get<1>(t), idx, result));
                op_out_vec_mutex[busIndex].unlock();
            }

        op_in_vec_mutex[busIndex].unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void* Daemon::blockingProcess(OpFunc of, void* data){
    if (single_threaded) return nullptr;
    // push datafunc tuple to data_in_vec
    if (availableIDs.empty()) std::terminate();
    of.PID = availableIDs.front();
    availableIDs.pop();
    data_in_mutex.lock();
    data_in_vec.push_back(DATA_IN(of, data));
    data_in_mutex.unlock();

    // check for packaged results in data_out_vec
    while (true) {
        // encapsulate with atomic bool check here..
        data_out_mutex.lock();
            for (auto it = data_out_vec.begin(); it != data_out_vec.end(); it++){
                DATA_OUT t = *it;
                if (of.PID == std::get<0>(t)){
                    data_out_vec.erase(it);
                    data_out_mutex.unlock();
                    return std::get<1>(t);
                }
            }
        data_out_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

}
