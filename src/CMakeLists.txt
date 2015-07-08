
add_definitions(-DVERSION="${LOMOCO_VERSION}" -DPACKAGE="${CMAKE_PROJECT_NAME}")
add_executable(lomoco lomoco.c cmdline.c)
target_link_libraries(lomoco usb)

