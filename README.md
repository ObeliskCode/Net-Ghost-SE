# Obelisk Engine (Windows/Linux)
# ![](https://github.com/n6garcia/Obelisk-3D-Engine/blob/main/demo.png)
Obelisk Engine is a lightweight C++ 3D Game Engine designed solely for the OpenGL API. The libraries included by Obelisk Engine offer a feature complete C++ enviroment for making ANY video game feature now and in the future for MIT licensable projects. The Game Engine is designed for a Game I hope to release. The Game I hope to release has an incredibly low graphics target so I hope to make an Engine which has little to no over-engineering (NO IMGUI!) and can handle any modern OpenGL feature efficiently. I hope that my project helps people stray away from over-engineered and over-priced messes like Unity and see that low-level graphics programming is actually quite easy. My code should be quite familiar to those who read https://learnopengl.com/. Obelisk Engine ultimately strives to have as few black boxes as possible for OpenGL Game Development.
### Author: Noel Garcia - n6garcia@ucsd.edu - https://noeldev.site/
## Libraries Included
- GLEW & GLFW (OpenGL Software Implementation)
- OpenAL-soft (OpenAL Software Implementation)
- Bullet Physics
- FreeType
- Boost
- GLM
- Assimp (Open Asset Import Library)
- Other Header Only Libraries
## Features Implemented 
#### Animated 3D models, Animation Blending, Particle System, Rigid Body Physics, Advanced Lighting, Directional/Point Shadows, Object Highlighting (Stencil Testing), Entity Component System, GUI, MultiThreaded Audio (OpenAL)

## Controls
WASD: Camera movement / walk

Space: Jump (when controlling rigid body)

Z: Lock camera to rigid body

X: Lock/Unlock cursor for mouse panning

O: toggle draw wires

P: Emit smoke particles

E: Select Physics Object

LEFT/RIGHT ARROW KEYS: Decrease/Increase interpolation between animations

F9: windowed

F10: fullscreen

## Build Instructions (Windows)
Note: Development versions of every library but Boost are included. Install Instructions included if you want to compile newer versions.

Note: GLFW & GLEW also need to be installed globally on the system. (unless you want to add them to /Libraries & link them yourself)
1. Download Visual Studio 2022.
2. Open GLEW_GLFW.zip and install both GLEW & GLFW on your system, Instructions inside Zip folder.
3. Install Assimp [https://assimp-docs.readthedocs.io/en/latest/about/installation.html] to Libraries (included)
```bash
git clone https://github.com/assimp/assimp.git
cd assimp
cmake CMakeLists.txt 
cmake --build .
cp -r include OBELISK_DIR/Libraries/include/
cp bin/Debug/assimp-vcxxx-mtd.dll "OBELISK_DIR/3D_OpenGL_Engine/assimp-vcxxx-mtd.dll"
cp lib/Debug/assimp-vcxxx-mtd.lib OBELISK_DIR/Libraries/lib/assimp-vcxxx-mtd.lib
```
4. Install Bullet Physics [https://github.com/bulletphysics/bullet3] to Libraries (included)
```bash
git clone https://github.com/bulletphysics/bullet3.git
cd BULLET_DIR
cmake .
open BULLET_PHYSICS.sln
change build target to release
Edit C/C++->Code Generation so that the compiled runtime library is set to Multi-threaded/MT
```
5. install Boost to Libraries/boost_x_xx_x [https://www.boost.org/]
6. install glm, copy /glm to Libraries/include/ [https://github.com/g-truc/glm] (included)
7. install freetype, copy /include contents to Libraries/Freetype and freetype.lib to Libraries/lib [https://freetype.org/] (included)
8. install openal-soft to Libraries/include and Libraries/lib [https://github.com/kcat/openal-soft] (included)
9. Within Visual Studio Update C/C++->General, Linker->General, & Linker->Input with all new paths and lib names
10. Download models from Obelisk-Models repo and copy to OBELISK_DIR/3D_OpenGL_Enginge/models/
11. Write Code!

## Build Instructions (Linux) WIP
To build on linux use the CodeBlocks Project (.cbp) in the 3D_OpenGL_Engine/ folder. 

Also build libassimp.a and put in Libraries_linux/lib.
