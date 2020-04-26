cmake_minimum_required(VERSION 3.16)
project(gst_tutorials C)

set(CMAKE_C_STANDARD 11)

add_executable(gst_tutorials
        test/test-app-sink.c)

target_link_libraries(${PROJECT_NAME} gstreamer-1.0)
target_link_libraries(${PROJECT_NAME} glib-2.0)
target_link_libraries(${PROJECT_NAME} gobject-2.0)
include_directories(/usr/include/gstreamer-1.0)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)