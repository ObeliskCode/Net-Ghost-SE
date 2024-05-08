#ifndef GUI_H
#define GUI_H

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef BUILD_WINDOWS
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "glm/glm.hpp"

#include "Shader.h"
#include "Globals.h"
#include <map>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class GUI {
public:
    // defines an class operation that lets clients access its unique instance.
    static GUI& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new GUI;
        return *instance;
    }
    GUI(const GUI&) = delete; // rule of three
    GUI& operator=(const GUI&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

private:
    GUI(); // no public constructor
    ~GUI(); // no public destructor
    static GUI* instance; // declaration class variable

    unsigned int VAO, VBO;

    std::map<char, Character> Characters;
};


#endif
