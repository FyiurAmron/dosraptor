cmake_minimum_required(VERSION 3.30)
project(src)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(src
        inc/ASK.INC
        inc/FILE0000.INC
        inc/FILE0001.INC
        inc/FILE0002.INC
        inc/FILE0003.INC
        inc/FILE0004.INC
        inc/HANGAR.INC
        inc/HELPWIN.INC
        inc/LOAD.INC
        inc/LOADCOMP.INC
        inc/LOCKER.INC
        inc/MAIN.INC
        inc/MSG.INC
        inc/OKREG.INC
        inc/OPTS.INC
        inc/REGIST.INC
        inc/SHIPCOMP.INC
        inc/STORE.INC
        inc/WINGAME.INC

        ANIMS.C
        ANIMS.H
        BONUS.C
        BONUS.H
        DERBIS.C
        ENEMY.C
        ENEMY.H
        ESHOT.C
        ESHOT.H
        FLAME.C
        FLAME.H
        FX.C
        FX.H
        HELP.C
        HELP.H
        INPUT.C
        INPUT.H
        INTRO.C
        INTRO.H
        LOADSAVE.C
        LOADSAVE.H
        MAP.H
        MOVIE.C
        MOVIE.H
        OBJECTS.C
        OBJECTS.H
        PUBLIC.H
        RAP.C
        SHADOWS.C
        SHADOWS.H
        SHOTS.C
        SHOTS.H
        STORE.C
        STORE.H
        TILE.C
        TILE.H
        WINDOWS.C
        WINDOWS.H
        tile_a.c
        tile_a.h

        ansi_esc.h
        utils.c
        utils.h
)
