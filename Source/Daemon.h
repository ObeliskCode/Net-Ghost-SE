#ifndef DAEMON_H
#define DAEMON_H

#include <thread>
#include <vector>
#include <tuple>
#include <queue>

#define SHORT_ID_MAX 65000

typedef void* (*OP)(void*);
typedef std::tuple<OP, void*> OP_TUPLE_IN;
typedef std::tuple<unsigned short, void*> OP_TUPLE_OUT;
typedef std::vector<OP_TUPLE_IN> OP_IN;
typedef std::vector<OP_TUPLE_OUT> OP_OUT;
typedef std::vector<OP_IN> OP_IN_VEC;
typedef std::vector<OP_OUT> OP_OUT_VEC;
typedef OP_IN_VEC& OP_IN_VEC_REF;
typedef OP_OUT_VEC& OP_OUT_VEC_REF;

class OpFunc {
    public:
    unsigned short PID = 0;
    unsigned short dataCt;
    void (*Dispatch)(void*, OP_IN_VEC_REF, OP_OUT_VEC_REF);
    void* (*Operate)(void*);
    void* (*Package)(void**, unsigned short);
};

typedef std::tuple<OpFunc, void*> DATA_IN;
typedef std::tuple<unsigned short, void*> DATA_OUT;
typedef std::vector<DATA_IN> DATA_IN_VEC;
typedef std::vector<DATA_OUT> DATA_OUT_VEC;

class BB3DFunc : public OpFunc {
    public:
    BB3DFunc(unsigned short ct) {
        dataCt = ct;
    }
    void (*Dispatch)(void*, OP_IN_VEC_REF, OP_OUT_VEC_REF) = [](void* data, OP_IN_VEC_REF op_in, OP_OUT_VEC_REF op_out) {
        return;
    };

    void* (*Operate)(void*) = [](void* data) -> void* {
        return nullptr;
    };

    void* (*Package)(void**, unsigned short) = [](void** data, unsigned short dataCt) -> void* {
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

    DATA_IN_VEC data_in;
    std::mutex data_in_mutex;
    DATA_OUT_VEC data_out;
    std::mutex data_out_mutex;

    OP_IN_VEC op_in;
    std::mutex op_in_mutex[32];
    OP_OUT_VEC op_out;
    std::mutex op_out_mutex[32];

    void pollDaemon();
    void pollWorker(unsigned int workerCt);

    void* blockingProcess(OpFunc, void*);
    //unsigned short sendProcess(OpFunc, void*);
    //void* recProcess(unsigned short);

    std::queue<unsigned short> availableIDs;

    //may return 0 when not able to detect
    unsigned int m_processor_count = std::thread::hardware_concurrency();

private:
    Daemon(); // no public constructor
    ~Daemon(); // no public destructor
    static Daemon* instance; // declaration class variable

};
#endif
