#ifndef DAEMON_H
#define DAEMON_H

#include <thread>
#include <vector>

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

    std::thread daemon;

    std::vector<std::thread> Workers;

    void pollDaemon();
    void pollWorker();

    //may return 0 when not able to detect
    unsigned int m_processor_count = std::thread::hardware_concurrency();

private:
    Daemon(); // no public constructor
    ~Daemon() = default; // no public destructor
    static Daemon* instance; // declaration class variable

};
#endif
