#ifndef DAEMON_H
#define DAEMON_H

#include <thread>
#include <vector>
#include <tuple>

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

    std::atomic_bool threadStopped = false;
    std::thread daemon;
    std::vector<std::thread> Workers;

    std::vector<std::tuple<short, void*>> chunkin;
    std::vector<std::tuple<short, void*>> chunkout;

    std::vector<std::vector<std::tuple<std::tuple<short, short>, void*>>> busin;
    std::vector<std::vector<std::tuple<std::tuple<short, short>, void*>>> busout;

    void pollDaemon();
    void pollWorker(unsigned int workerCt);

    void* blockingChunk(std::tuple<short, void*>);
    short sendChunk(std::tuple<short, void*>);
    void* recChunk(short ID);

    //may return 0 when not able to detect
    unsigned int m_processor_count = std::thread::hardware_concurrency();

private:
    Daemon(); // no public constructor
    ~Daemon(); // no public destructor
    static Daemon* instance; // declaration class variable

};
#endif
