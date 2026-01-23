cmake_minimum_required(VERSION 3.16)
project(widefetch_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(widefetch
    src/main.c
    src/fmt.c
    src/logo.c
    src/util.c
    src/modules/os.c
    src/modules/host.c
    src/modules/cpu.c
    src/modules/gpu.c
    src/modules/memory.c
    src/modules/uptime.c
)

target_include_directories(widefetch PRIVATE include)
