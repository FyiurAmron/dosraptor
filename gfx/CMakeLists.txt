cmake_minimum_required(VERSION 3.30)
project(gfx)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(gfx
        DPMIAPI.C
        DPMIAPI.H
        exitapi.c
        EXITAPI.H
        GFXAPI.C
        GFXAPI.H
        GLBAPI.C
        GLBAPI.H
        IMSAPI.C
        IMSAPI.H
        KBDAPI.C
        KBDAPI.H
        KEYS.H
        PREFAPI.C
        PREFAPI.H
        PTRAPI.C
        PTRAPI.H
        swdapi.c
        SWDAPI.H
        TSMAPI.C
        TSMAPI.H
        types.h
        VMEMAPI.C
        VMEMAPI.H
        common.h
)
