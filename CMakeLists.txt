cmake_minimum_required(VERSION 3.1)

project(lomoco)

file (STRINGS "ChangeLog" LOMOCO_VERSION LIMIT_COUNT 1 REGEX "lomoco \\([0-9.]+\\)")
string(REGEX MATCH "[0-9.]+" LOMOCO_VERSION ${LOMOCO_VERSION})

add_subdirectory(src)

