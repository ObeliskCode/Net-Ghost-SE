# Obelisk Engine

### 3D Game Engine - C++/OpenGL

### Author: Noel Garcia - n6garcia@ucsd.edu - https://noeldev.site/

## Features Implemented 
#### Animated 3D models, Particle System, Rigid Body Physics, Advanced Lighting/Shadows

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

## Build Instructions 
1. Download Visual Studio 2022.
2. Open GLEW_GLFW.zip and install both GLEW & GLFW on your system, Instructions inside Zip folder.
3. Install Assimp [https://assimp-docs.readthedocs.io/en/latest/about/installation.html] to Libraries
```bash
git clone https://github.com/assimp/assimp.git
cd assimp
cmake CMakeLists.txt 
cmake --build .
cp -r include OBELISK_DIR/Libraries/assimp/
cp bin/Debug/assimp-vc143-mtd.dll "OBELISK_DIR/3D OpenGL Engine/assimp-vc143-mtd.dll"
cp lib/Debug/assimp-vc143-mtd.lib OBELISK_DIR/Libraries/assimp/lib/assimp-vc143-mtd.lib
cp lib/Debug/assimp-vc143-mtd.exp OBELISK_DIR/Libraries/assimp/lib/assimp-vc143-mtd.exp
```
4. Install Bullet Physics [https://github.com/bulletphysics/bullet3] to Libraries
```bash
#Click on build_visual_studio_vr_pybullet_double.bat and open build3/vs2010/0_Bullet3Solution.sln
#When asked, convert the projects to a newer version of Visual Studio.
#Change Build to Release x64
#Edit C/C++->Code Generation so that Runtime Library is set to Multi-threaded/MT
#Right Click and Build only projects BulletCollision,BulletDynamics & LinearMath
#Copy lib files to Libraries/bullet/lib/ and source files to Libraries/bullet/include/ (files within lib/ and src/ folders)
```
5. Within Visual Studio Update C/C++->General, Linker->General, & Linker->Input with all new paths and lib names
6. Write Code!
