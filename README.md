# Obelisk Engine

### 3D Game Engine - C++/OpenGL

### Author: Noel Garcia - n6garcia@ucsd.edu - https://noeldev.site/

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

## Build Instructions (Windows Only)
Note: Older versions of every library but Boost are included.
1. Download Visual Studio 2022.
2. Open GLEW_GLFW.zip and install both GLEW & GLFW on your system, Instructions inside Zip folder.
3. Install Assimp [https://assimp-docs.readthedocs.io/en/latest/about/installation.html] to Libraries
```bash
git clone https://github.com/assimp/assimp.git
cd assimp
cmake CMakeLists.txt 
cmake --build .
cp -r include OBELISK_DIR/Libraries/include/
cp bin/Debug/assimp-vcxxx-mtd.dll "OBELISK_DIR/3D_OpenGL_Engine/assimp-vcxxx-mtd.dll"
cp lib/Debug/assimp-vcxxx-mtd.lib OBELISK_DIR/Libraries/lib/assimp-vcxxx-mtd.lib
```
4. Install Bullet Physics [https://github.com/bulletphysics/bullet3] to Libraries
```bash
#Click on build_visual_studio_vr_pybullet_double.bat and open build3/vs2010/0_Bullet3Solution.sln
#When asked, convert the projects to a newer version of Visual Studio.
#Change Build to Release x64
#Edit C/C++->Code Generation so that Runtime Library is set to Multi-threaded/MT
#Right Click and Build only projects BulletCollision,BulletDynamics & LinearMath
#Copy lib files to Libraries/lib/ and source folder to Libraries/include/ (files within lib/ and src/ folders)
```
5. install Boost to Libraries/boost_x_xx_x
6. install glm to Libraries/include/
7. Within Visual Studio Update C/C++->General, Linker->General, & Linker->Input with all new paths and lib names
8. Download models from Obelisk-Models repo and copy to OBELISK_DIR/3D_OpenGL_Enginge/models/
9. Write Code!
