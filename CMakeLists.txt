cmake_minimum_required(VERSION 3.20)
project(projet_c C)

set(CMAKE_C_STANDARD 99)

include_directories("C:/wamp64/bin/mariadb/mariadb10.6.5/include")
link_directories("C:/wamp64/bin/mariadb/mariadb10.6.5/lib")
link_libraries(libmariadb)


add_executable(projet_c main.c audio_func.h audio_func.c dbconf.h db_func.c db_func.h settings_func.c settings_func.h structures.h)

target_link_libraries(projet_c libmariadb)
