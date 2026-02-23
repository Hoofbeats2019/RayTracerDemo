# Ray Tracer Codebase - AI Coding Instructions

## Project Overview
This is a **COSC 363 Computer Graphics ray tracer** implementing physically-based ray tracing with advanced rendering features (reflections, refractions, transparency, shadows, and adaptive anti-aliasing).

**Architecture**: 
- **Core Engine**: [RayTracer.cpp](../RayTracer.cpp) contains the `trace()` recursive ray tracing algorithm and scene initialization
- **Scene Objects**: Abstract base class [SceneObject.h](../SceneObject.h) with implementations: [Sphere.h](../Sphere.h), [Plane.h](../Plane.h), [Cylinder.h](../Cylinder.h), [Cone.h](../Cone.h), [Circle.h](../Circle.h)
- **Ray System**: [Ray.h](../Ray.h) represents rays; `closestPt()` finds nearest object intersection
- **Texturing**: [TextureBMP.h](../TextureBMP.h) handles texture mapping for planes and spheres via UV coordinates

## Critical Data Structures

### Ray Class
- **p0** (glm::vec3): Ray origin/source point
- **dir** (glm::vec3): Unit direction vector (normalized)
- **hit** (glm::vec3): Intersection point with closest object
- **index** (int): Scene object array index of hit object (-1 = no hit)
- **dist** (float): Distance from p0 to hit point

**Key Method**: `closestPt(sceneObjects)` - compares ray with all scene objects, populates hit/index/dist

### SceneObject Interface
All objects inherit from `SceneObject` and must implement:
- `intersect(p0, dir)` → float: Returns distance to intersection (-1 = miss)
- `normal(pos)` → glm::vec3: Returns surface normal at position
- `lighting(lightPos, lightColor, viewVec, hitPoint)` → glm::vec3: Phong shading

**Material Properties** (configured via setters):
- `refl_`/`reflc_`: Reflectivity flag + coefficient (0-1)
- `refr_`/`refrc_`: Refractivity flag + coefficient  
- `tran_`/`tranc_`: Transparency flag + coefficient
- `spec_`/`shin_`: Specularity + shininess exponent

## Rendering Pipeline

### Main Loop: `display()` [RayTracer.cpp#L380]
1. Iterates NUMDIV×NUMDIV grid cells (500×500 = image resolution)
2. For each cell, calls `adaptiveSample()` to determine color
3. Cells are drawn as GL_QUADS with computed RGB values

### Ray Tracing: `trace(ray, step)` [RayTracer.cpp#L145]
**Recursive function** following this flow:

1. **Intersection Detection**: `ray.closestPt(sceneObjects)` finds nearest object
2. **Material-Based Rendering**:
   - **Phong Lighting**: If no shadow, applies `obj->lighting()` with dual light sources
   - **Shadow Handling**: If in shadow, calls `shadow()` helper; allows lighter shadows through transparent/refractive objects
   - **Transparency**: If `obj->isTransparent()` and `step < MAX_STEPS`, traces through object with coefficient blending
   - **Reflection**: If `obj->isReflective()`, computes `glm::reflect()` and recursively traces reflected ray
   - **Refraction**: If `obj->isRefractive()`, uses Snell's law with `glm::refract()`; checks for total internal reflection
3. **Spotlight**: Adds spotlight contribution if hit point is within cutoff angle
4. **Fog**: Applies linear fog based on distance (z coordinate)

**Recursion Control**: `step` parameter limits depth; `MAX_STEPS = 25` prevents stack overflow

### Anti-Aliasing: `adaptiveSample()` [RayTracer.cpp#L330]
Adaptive spatial sampling:
- Samples 4 corners of pixel cell
- Computes color variance
- If variance > `COLOR_THRESHOLD (0.4f)` and `depth < MAX_DEPTH (1)`, recursively subdivides into 4 quadrants
- Otherwise, uses average color

## Scene Initialization: `initialize()` [RayTracer.cpp#L360]
Creates scene objects by index (0-25+):
- **0-5**: Box walls (6 planes)
- **6-9**: First sphere/cylinder pair (transformed with translation/rotation/scale)
- **10-14**: Table with 4 legs + surface (cylinders + plane)
- **15-26**: Additional objects on table (cylinders, cones, spheres with transparency/refraction)

**Pattern**: Objects are created with `new`, added to global `vector<SceneObject*> sceneObjects`

## Essential Global Parameters [RayTracer.cpp#L27-40]
```cpp
const int NUMDIV = 500;      // Image resolution
const int MAX_STEPS = 25;    // Recursion depth limit
const float EDIST = 30.0;    // Eye distance from image plane
const float XMIN/XMAX/YMIN/YMAX = ±20.0;  // View frustum
const float COLOR_THRESHOLD = 0.4f;  // AA variance trigger
const int MAX_DEPTH = 1;     // Max AA subdivision depth
```

## Key Coding Patterns

### Adding New Scene Objects
```cpp
// 1. Create object
Sphere *newSphere = new Sphere(glm::vec3(x, y, z), radius);
newSphere->setColor(glm::vec3(r, g, b));

// 2. Set material properties
newSphere->setReflectivity(true, 0.8f);  // reflective with coeff 0.8
newSphere->setTransparency(true, 0.3f);  // transparent with coeff 0.3

// 3. Add to scene
sceneObjects.push_back(newSphere);
```

### Texture Mapping
- **Plane textures**: Compute UV via `(hit.z - minZ) / (maxZ - minZ)` and `(hit.y - minY) / (maxY - minY)`
- **Sphere textures**: Use `sphere->getTextureCoordinates(hitPoint)` for spherical UV mapping
- Access texture via `textureEarth.getColorAt(u, v)` (returns glm::vec3 RGB)

### Lighting Calculations
- **Dual lights** at positions `lightPos1/2` with color `lightColor(0.5)`
- Shadow rays check if `shadowRay.dist < glm::length(lightVec)` to determine shadowing
- Spotlight centered at `spotlightPos` with cutoff angle; uses `glm::cos(glm::radians(4.0f))`

## Build & Execution
```bash
mkdir build && cd build
cmake ..
make
./RayTracer.out
```

**Dependencies**: OpenGL, GLUT, GLM (installed via CMake find_package)

## Common Modifications
- **Change resolution**: Modify `NUMDIV` (impacts render time quadratically)
- **Adjust recursion depth**: Change `MAX_STEPS` for more reflections/refractions
- **Modify lighting**: Adjust `lightPos1/2`, `lightColor`, ambient `AMBIENT` constant
- **Add texture**: Load new BMP in `initialize()` via `TextureBMP("filename.bmp")`
- **Debug intersections**: Check `ray.index == -1` for no-hit cases; add console logging in `trace()`

## Debugging Tips
- Objects at scene indices are used throughout `trace()` via hardcoded checks (e.g., `if (ray.index == 24)` for chequered pattern)
- Modify object indices carefully if reordering scene objects
- Fog effect is linear; adjust `fogStart`/`fogEnd` in fog formula if visibility changes needed
- Adaptive AA can cause artifacts; increase `COLOR_THRESHOLD` to reduce subdivisions
