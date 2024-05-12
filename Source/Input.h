#ifndef INPUT_H
#define INPUT_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input {
public:
    // defines an class operation that lets clients access its unique instance.
    static Input& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new Input;
        return *instance;
    }
    Input(const Input&) = delete; // rule of three
    Input& operator=(const Input&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }
    // existing interface goes here
    bool getValue(unsigned int key) {
        if (key >= GLFW_KEY_LAST) return false;
        return keys[key];
    }
    void setValue(unsigned int key, bool value) {
        if (key >= GLFW_KEY_LAST) return;
        keys[key] = value;
    }
private:
    Input() = default; // no public constructor
    ~Input() = default; // no public destructor
    static Input* instance; // declaration class variable

    bool keys[GLFW_KEY_LAST] = { false };
};
#endif
