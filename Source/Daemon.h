#ifndef DAEMON_H
#define DAEMON_H

#include <thread>
#include <vector>
#include <tuple>

class OpFunc {
    public:
    unsigned short ID = 0;
    unsigned short dataCt;
    unsigned short (*Dispatch)(void*);
    void* (*Operate)(void*);
    void* (*Package)(void**);
};

class BB3DFunc : public OpFunc {
    public:
    BB3DFunc(unsigned short ct) {
        dataCt = ct;
    }
    unsigned short (*Dispatch)(void*) = [](void* data) -> unsigned short {
        return 0;
    };

    void* (*Operate)(void*) = [](void* data) -> void* {
        return nullptr;
    };

    void* (*Package)(void**) = [](void** data) -> void* {
        return nullptr;
    };

};

// gross class (to be refactored), breaks if you include it in files you include above!
class Daemon {
public:
    // defines an class operation that lets clients access its unique instance.
    static Daemon& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new Daemon();
        return *instance;
    }
    Daemon(const Daemon&) = delete; // rule of three
    Daemon& operator=(const Daemon&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    bool single_threaded = false;

    std::atomic_bool threadStopped = false;
    std::thread daemon;
    std::vector<std::thread> Workers;

    std::vector<std::tuple<OpFunc, void*>> data_in;
    std::mutex data_in_mutex;
    std::vector<std::tuple<short, void*>> data_out;
    std::mutex data_out_mutex;

    std::vector<std::vector<std::tuple<void* (*)(void*), void*>>> op_in;
    std::mutex op_in_mutex[32];
    std::vector<std::vector<std::tuple<short, void*>>> op_out;
    std::mutex op_out_mutex[32];

    void pollDaemon();
    void pollWorker(unsigned int workerCt);

    void* blockingProcess(std::tuple<OpFunc, void*>);
    unsigned short sendProcess(std::tuple<OpFunc, void*>);
    void* recProcess(unsigned short ID);

    //may return 0 when not able to detect
    unsigned int m_processor_count = std::thread::hardware_concurrency();

private:
    Daemon(); // no public constructor
    ~Daemon(); // no public destructor
    static Daemon* instance; // declaration class variable

};
#endif
