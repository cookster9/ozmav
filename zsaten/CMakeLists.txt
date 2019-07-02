cmake_minimum_required (VERSION 3.0)

project (zSatenWindows)

include_directories("${PROJECT_SOURCE_DIR}/badrdp")
include_directories("${PROJECT_SOURCE_DIR}/misaka")

add_subdirectory(badrdp)
add_subdirectory(misaka)

set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/vcpkg-master/scripts/buildsystems/vcpkg.cmake")

find_package(GLEW REQUIRED)
find_package(PNG)
find_package(zlib)

add_executable(zsaten 
__linux.c
__win32.c
camera.c
hud.c
main.c
memwatch.c
mips-eval.c
oz.c
render.c
shared.c
zactors.c
zelda.c
zold.c
editor.c)

target_link_libraries(zsaten
badrdp
misaka
GLEW::GLEW
)