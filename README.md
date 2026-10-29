# OpenGL C++ Physics & Simulation Engine
A 2D/3D physics engine and real-time interactive simulation environment built with C++, OpenGL API, and using GLSL. Demonstrates 2D and 3D rigid body particle dynamics, Verlet integration 2D cloth physics, and batched rendering.

***

## Demos

[![Watch this video](https://img.youtube.com/vi/ZxczLxmJR1Y/maxresdefault.jpg)](https://www.youtube.com/watch?v=ZxczLxmJR1Y)
*2D Physics Demo (click to watch)*
<br><br>

[![Watch this video](https://img.youtube.com/vi/2b2MA5n4yg0/maxresdefault.jpg)](https://www.youtube.com/watch?v=2b2MA5n4yg0)
*3D Physics Demo (click to watch)*

***

## Features & Architecture

* **Physics & Simulation Systems:**
  * **2D Rigid Body / Circle Collisions:** Spatial bounds checking and impulse-based impulse/penetration resolution.
  * **2D Verlet Cloth Simulation:** Distance-constraint solver for dynamic fabric sheet rendering.
  * **3D Sphere Collisions:** Penalty-based / impulse collision detection and dynamic response in 3D space.
* **Engine Architecture:**
  * **Component-System Pattern:** Decoupled scene graph hierarchy separating transformation data, renderables, and physics properties.
  * **Modular Scene Manager:** Clean interface for swapping, updating, and resetting test simulation scenarios.
  * **OpenGL Renderer:** Batched renderer pipeline for efficient particle, constraint line, and mesh primitive drawing.
  * **Interactive UI:** Built-in ImGui inspector for real-time physics tuning (gravity, restitution, stiffness, step rates).

***

## Tech Stack & Dependencies

* **Language & Build System:** C++20, CMake (3.20+)
* **Graphics API:** OpenGL
* **Shading Language:** GLSL
* **Libraries:** GLFW, GLAD, GLM, Dear ImGui, stb_image

***