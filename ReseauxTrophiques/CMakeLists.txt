cmake_minimum_required(VERSION 3.23)
project(ReseauxTrophiques C)

set(CMAKE_C_STANDARD 99)

add_executable(ReseauxTrophiques main.c sousProgramme.c Header.h simul.c)
target_link_libraries(ReseauxTrophiques -lmingw32 -lalleg44 -lliblogg)
