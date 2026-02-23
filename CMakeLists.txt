cmake_minimum_required(VERSION 2.8...3.5)

project(lab8)

add_executable(RayTracer.out RayTracer.cpp Ray.cpp SceneObject.cpp Sphere.cpp Plane.cpp Cylinder.cpp Cone.cpp Circle.cpp TextureBMP.cpp)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

# Prefer the modern imported target provided by glm (glm::glm).
# If the imported target isn't available, fall back to using the include
# directory provided by the find_package config/module.
if (TARGET glm::glm)
	target_link_libraries(RayTracer.out PRIVATE glm::glm)
elseif (DEFINED GLM_INCLUDE_DIR)
	include_directories(${GLM_INCLUDE_DIR})
endif()

# Use keyword form to match the earlier `PRIVATE` usage above
target_link_libraries(RayTracer.out PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

