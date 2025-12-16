cmake_minimum_required(VERSION 3.27)
project(LGL LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)

check_c_compiler_flag("-msse4.1" COMPILER_SUPPORTS_SSE4_1)

file(GLOB SRC_FILES src/*.c)

if (WIN32)
    set(PLATFORM_FILE src/platform/PlatformWindows.c)
    set(PLATFORM_LIBS Winmm)
elseif (UNIX)
    set(PLATFORM_FILE src/platform/PlatformLinux.c)
    set(PLATFORM_LIBS X11 Xext m)
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
        include
)
if(COMPILER_SUPPORTS_SSE4_1)
    target_compile_options(LGL PRIVATE -msse4.1)
    target_compile_definitions(LGL PRIVATE __SSE4_1__=1)
endif()

add_executable(Main test.c)
target_include_directories(Main PUBLIC include)
target_link_libraries(Main PUBLIC LGL)

add_executable(PerfTest main.c)
target_include_directories(PerfTest PUBLIC include)
target_link_libraries(PerfTest PUBLIC LGL)
