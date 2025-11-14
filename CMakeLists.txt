cmake_minimum_required(VERSION 3.27)
project(LGL LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES src/*.c)

if (WIN32)
    set(PLATFORM_FILE src/platform/PlatformWindows.c)
elseif (UNIX)
    set(PLATFORM_FILE src/platform/PlatformLinux.c)
else ()
    message("Invalid platform")
endif ()

add_library(LGL STATIC
        ${SRC_FILES}
        ${PLATFORM_FILE}
)
target_link_libraries(LGL PUBLIC Winmm)
target_include_directories(LGL PUBLIC
        include external
)
