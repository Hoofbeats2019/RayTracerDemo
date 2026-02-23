**RayTracyDemo — README**

**Program Description**

This is a physically-based ray tracer implementing advanced rendering techniques for Computer Graphics. The program traces rays through a 3D scene to compute photorealistic images by simulating light behavior in the environment.

**Core Features:**
- **Lighting & Shading**: Dual point light sources with Phong illumination model, computing diffuse and specular reflections for realistic material appearance.
- **Reflections & Refractions**: Recursive ray tracing supporting reflective surfaces (mirrors with 0.8–0.9 coefficient) and refractive media (glass with configurable refractive index ~1.5). Handles total internal reflection detection.
- **Transparency**: Semi-transparent objects with customizable opacity coefficients allowing light to pass through with blending.
- **Advanced Shadow Rendering**: Multiple shadow ray computation with special handling for light attenuation through transparent/refractive objects, creating softer shadow effects.
- **Texture Mapping**: BMP texture support via UV coordinate mapping on planar surfaces (butterfly image) and spherical objects (Earth texture with spherical projection).
- **Adaptive Anti-Aliasing**: Intelligent spatial sampling algorithm that detects high-variance pixels and recursively subdivides them (up to 2 levels) for superior edge quality without unnecessary computation.
- **Spotlight Effects**: Directional spotlight with configurable angle cutoff (~4°) for focused illumination zones.
- **Atmospheric Fog**: Linear depth-based fog for atmospheric perspective and visual depth cues.
- **Scene Objects**: Spheres, planes, cylinders, cones, and circles with full affine transformations (scaling, rotation, translation).

**Technical Details:**
The renderer discretizes the 500×500 view plane into cells, traces primary rays per pixel, and recursively computes reflections/refractions up to 25 depth levels. Built-in scene includes a box environment, textured table with objects, and various demonstrative shapes. Output renders in real-time via OpenGL/GLUT.

**Quick Start**
- **Clone / open:** open this project folder in your IDE/terminal.
- **Configure & build:**

```bash
cmake -S . -B build
cmake --build build
```

- **Run:**

```bash
./build/RayTracer.out
```

**Platform prerequisites**
This project depends on the following system libraries:
- OpenGL
- GLUT / FreeGLUT
- GLM (OpenGL Mathematics)
- CMake and a C++ compiler (g++, clang++)

Below are detailed install instructions per platform.

**macOS (Homebrew)**
- Install required packages:

```bash
brew install cmake glm freeglut
```

- Notes:
  - Apple provides legacy OpenGL headers; Homebrew supplies freeglut and glm.
  - If you installed glm earlier, CMake should find it via `find_package(glm REQUIRED)`.

**Ubuntu / Debian**

```bash
sudo apt update
sudo apt install build-essential cmake libglm-dev freeglut3-dev libglu1-mesa-dev mesa-common-dev
```

**Windows**
- Recommended via vcpkg (works with Visual Studio / MSVC or with CMake toolchain):

```powershell
# from project root
git clone https://github.com/Hoofbeats2019/RayTracerDemo.git
cd vcpkg
./bootstrap-vcpkg.sh  # or .\bootstrap-vcpkg.bat on Windows
./vcpkg install glm freeglut
# then configure CMake with -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Alternative: install libraries via Chocolatey or fetch prebuilt binaries for GLUT/FreeGLUT and ensure glm headers are on include path.

**CMake configuration notes**
- The project uses `find_package` for OpenGL, GLUT and GLM in [CMakeLists.txt](CMakeLists.txt).
- The preferred modern usage is to link the imported target `glm::glm` (provided by GLM config packages). Example (present in the project):

- In [CMakeLists.txt](CMakeLists.txt) we use:

```cmake
find_package(glm REQUIRED)

if (TARGET glm::glm)
    target_link_libraries(RayTracer.out PRIVATE glm::glm)
elseif (DEFINED GLM_INCLUDE_DIR)
    include_directories(${GLM_INCLUDE_DIR})
endif()

target_link_libraries(RayTracer.out PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
```

- If CMake cannot find GLM:
  - Ensure `glm` is installed (see platform instructions above).
  - You can set `GLM_DIR` or `GLM_INCLUDE_DIR` when invoking CMake:

```bash
cmake -S . -B build -DGLM_INCLUDE_DIR=/path/to/glm
```

or provide `-DGLM_DIR` to point to a GLM package config folder if you have one.

**CMake minimum version**
- The repository currently uses the backward-compatible `cmake_minimum_required` syntax. If you prefer modern CMake features, update the top of [CMakeLists.txt](CMakeLists.txt) to a newer minimum (for example `3.10`):

```cmake
cmake_minimum_required(VERSION 3.10)
```

After updating, verify your local CMake version (`cmake --version`).

**Common build & link errors and fixes**
- "GLM not found": install `glm` via your package manager or point `GLM_INCLUDE_DIR` to the header location.
- "target_link_libraries signature" error: use consistent keyword form and prefer `PRIVATE`/`PUBLIC`/`INTERFACE`. The project already uses the keyword form.
- If your system has multiple glm installations, prefer an imported target (`glm::glm`) and remove manual `include_directories`/`link_libraries` for glm.

**Project usage & tweakable constants**
- Source entry: [RayTracer.cpp](RayTracer.cpp)
- Resolution: change `NUMDIV` in `RayTracer.cpp` to change render resolution (default 500).
- Recursion depth: change `MAX_STEPS` in `RayTracer.cpp` to allow more reflections/refractions.
- Adaptive AA threshold: `COLOR_THRESHOLD` and `MAX_DEPTH` in `RayTracer.cpp` control sampling.
- Textures: `Butterfly.bmp` and `Earth.bmp` are loaded by `TextureBMP` in `initialize()`; place these BMP files in the project root or adjust paths.

**Recommended workflow**
1. Ensure dependencies are installed (see platform section).
2. From project root run:

```bash
cmake -S . -B build
cmake --build build --config Release
```

3. Run the executable:

```bash
./build/RayTracer.out
```

4. Edit `RayTracer.cpp` to change the scene, resolution or other constants and rebuild.

**Troubleshooting tips**
- If display window opens black or closes immediately, check OpenGL/GLUT installation and that your system supports the required GL profile.
- If textures do not show, ensure BMP files exist and are readable in the working directory.
- For CMake diagnostic output, run:

```bash
cmake -S . -B build -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build --verbose
```

**Where to change code for common edits**
- Scene setup & objects: [RayTracer.cpp](RayTracer.cpp#L360)
- Trace algorithm: `trace()` in [RayTracer.cpp](RayTracer.cpp#L145)
- Ray helper: [Ray.h](Ray.h) / [Ray.cpp](Ray.cpp)
- Object implementations: [Sphere.*](Sphere.h) / [Plane.*](Plane.h) / [Cylinder.*](Cylinder.h) / [Cone.*](Cone.h) / [Circle.*](Circle.h)

**Licence & attribution**
- This README does not change source licensing; your project likely follows your university/course rules. Keep attribution for third-party code (GLM, GLUT) as required by their licenses.

If you want, I can:
- Update `CMakeLists.txt` to require a modern CMake version and use `target_include_directories` instead of `include_directories`.
- Add a small `Makefile` wrapper or script to simplify build/run steps.

Happy to make those changes — tell me which option you prefer.
