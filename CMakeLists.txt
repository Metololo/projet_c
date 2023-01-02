cmake_minimum_required(VERSION 3.20)
project(projet_c C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories("E:/wamp64/bin/mariadb/mariadb10.6.5/include" ${GTK_INCLUDE_DIRS})
link_directories("E:/wamp64/bin/mariadb/mariadb10.6.5/lib" ${GTK_LIBRARY_DIRS})
link_libraries(libmariadb)

add_definitions(${GTK_CFLAGS_OTHER})
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

add_executable(projet_c main.c audio_func.h audio_func.c dbconf.h db_func.c db_func.h settings_func.c settings_func.h structures.h radiofunc.c struct_func.c struct_func.h radiofunc.h gtk_functions.c gtk_functions.h)

target_link_libraries(projet_c libmariadb ${GTK_LIBRARIES})
