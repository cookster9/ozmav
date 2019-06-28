cmake_minimum_required (VERSION 3.0)

project (zSatenWindows)

add_library(badrdp STATIC 
combine.c
crc.c
dlparse.c
dump.c
f3d.c
f3dex.c
f3dex2.c
gdp.c
gsp.c
macro.c
matrix.c
opengl.c
rdp.c
)