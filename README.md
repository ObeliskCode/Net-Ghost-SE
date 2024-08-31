# Net Ghost Science Engine (Linux)
Net Ghost Science Engine is a lightweight C++ 3D Game Engine designed solely for the OpenGL API. The libraries included by Net Ghost Science Engine offer a feature complete C++ enviroment for making almost any video game feature now and in the future for MIT licensable projects. The Engine is being built to support artificial intelligence tooling built into the engine. I plan to develop a cmd only version of NGSE for 0 overhead in training image based artificial intelligence. I plan to support networking only through p2p to hopefully try to opensource the engines networking development. 

## BUILD INSTRUCTIONS
```bash
python build.py
```
## Features Implemented 
#### Animated 3D models, Animation Blending, Particle System, Rigid Body Physics, Advanced Lighting, Directional/Point Shadows, Object Highlighting (Stencil Testing), Entity Component System, GUI, MultiThreaded Audio (OpenAL)

## Libraries Included
- GLEW & GLFW (OpenGL Software Implementation)
- OpenAL-soft (OpenAL Software Implementation)
- Bullet Physics
- FreeType
- Boost
- GLM
- Assimp (Open Asset Import Library)
- Other Header Only Libraries

## Controls
WASD: Camera movement / walk,
Space: Jump (when controlling rigid body),
Z: Lock camera to rigid body,
X: Lock/Unlock cursor for mouse panning,
O: toggle draw wires,
P: Emit smoke particles,
E: Select Physics Object,
LEFT/RIGHT ARROW KEYS: Decrease/Increase interpolation between animations,
F9: windowed,
F10: fullscreen
