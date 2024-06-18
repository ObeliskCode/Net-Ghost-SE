#ifndef DAEMON_H
#define DAEMON_H

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

private:
    Daemon() { } // no public constructor
    ~Daemon() = default; // no public destructor
    static Daemon* instance; // declaration class variable

};
#endif
