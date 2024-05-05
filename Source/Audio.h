#include <AL/al.h>
#include <AL/alc.h>


#include "dr_wav.h"

#include <iostream>
#include <vector>

class Audio {
public:
    // defines an class operation that lets clients access its unique instance.
    static Audio& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new Audio;
        return *instance;
    }
    Audio(const Audio&) = delete; // rule of three
    Audio& operator=(const Audio&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    void playSound();


private:
    Audio(); // no public constructor
    ~Audio(); // no public destructor
    static Audio* instance; // declaration class variable

    ALuint monoSoundBuffer;
    ALuint stereoSoundBuffer;
    ALuint monoSource;
    ALuint stereoSource;

    ALCcontext* context;
    ALCdevice* device;

};
