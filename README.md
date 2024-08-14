# Obelisk Engine (Windows/Linux)
Obelisk Engine is a lightweight C++ 3D Game Engine designed solely for the OpenGL API. The libraries included by Obelisk Engine offer a feature complete C++ enviroment for making ANY video game feature now and in the future for MIT licensable projects. The Game Engine is designed for a Game I hope to release. The Game I hope to release has an incredibly low graphics target so I hope to make an Engine which has little to no over-engineering and can handle any modern OpenGL feature efficiently. I hope that my project helps people stray away from over-engineered and over-priced messes like Unity and see that low-level graphics programming is actually quite easy. My code should be quite familiar to those who read https://learnopengl.com/. Obelisk Engine ultimately strives to have as few black boxes as possible for OpenGL Game Development.
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

## BUILD INSTRUCTIONS
```bash
python build.py
```
