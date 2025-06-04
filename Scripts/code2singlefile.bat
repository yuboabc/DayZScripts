@echo off

set dir_1_core=%~dp01_core
set dir_2_gamelib=%~dp02_gamelib
set dir_3_game=%~dp03_game
set dir_4_world=%~dp04_world
set dir_5_mission=%~dp05_mission

for /f "delims=" %%a in ('dir /b /s "%dir_1_core%\*.c"') do (
    type "%%a" >> 1_core.c
)
for /f "delims=" %%a in ('dir /b /s "%dir_2_gamelib%\*.c"') do (
    type "%%a" >> 2_gamelib.c
)
for /f "delims=" %%a in ('dir /b /s "%dir_3_game%\*.c"') do (
    type "%%a" >> 3_game.c
)
for /f "delims=" %%a in ('dir /b /s "%dir_4_world%\*.c"') do (
    type "%%a" >> 4_world.c
)
for /f "delims=" %%a in ('dir /b /s "%dir_5_mission%\*.c"') do (
    type "%%a" >> 5_mission.c
)