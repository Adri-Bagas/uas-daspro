@echo off
REM Windows Build Script using GCC (MinGW)


gcc main.c feat/*.c db/*.c sqlite/sqlite3.c -o fmcon.exe -I.\sqlite

IF %ERRORLEVEL% EQU 0 (
    echo Compilation successful.
    echo Run "fmcon.exe init" to start.
) ELSE (
    echo Compilation failed.
)
pause