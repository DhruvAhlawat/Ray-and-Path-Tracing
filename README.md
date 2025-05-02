# Path tracing
---
## how to run
Make sure that [glm](https://github.com/g-truc/glm) and [SDL2](https://www.libsdl.org/) are installed. Ideally, these should be installed by your package manager rather than manually (at least, if you are on Linux or Mac).

Then compile the code using the standard CMake procedure:

- The first time, run `cmake -B build` from the project root to create a `build/` directory and initialize a build system there.
- Then, every time you want to compile the code, run `cmake --build build` (again from the project root). Then the example programs will be created under `build/`.
## Implementation
 uses Blinn-Phong for rough specular surfaces, and fresnel approximations for transparent dielectric crystals.

## sample images
![image](https://github.com/user-attachments/assets/c2f097bb-519e-466a-88d9-c3b1dd7628ab)
![image](https://github.com/user-attachments/assets/cc716898-ab24-4cd0-a150-fefbb09c1378)
![image](https://github.com/user-attachments/assets/9007ab94-2e40-461a-bbbb-e2a237c7dbe3)

