cmake_minimum_required(VERSION 3.27)
project(LGL LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES src/*.c)

if (WIN32)
    set(PLATFORM_FILE src/platform/PlatformWindows.c)
    set(PLATFORM_LIBS Winmm)
elseif (UNIX)
    set(PLATFORM_FILE src/platform/PlatformLinux.c)
    set(PLATFORM_LIBS X11 m)
else ()
    message("Invalid platform")
endif ()

find_package(Freetype REQUIRED)
find_package(harfbuzz REQUIRED)

add_library(LGL STATIC
        ${SRC_FILES}
        ${PLATFORM_FILE}
)
target_link_libraries(LGL PRIVATE ${PLATFORM_LIBS} Freetype::Freetype harfbuzz)
target_include_directories(LGL PRIVATE
        include external
)

add_executable(Main main.c)
target_include_directories(Main PUBLIC include)
target_link_libraries(Main PUBLIC LGL)
