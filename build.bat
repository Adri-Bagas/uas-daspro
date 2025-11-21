@echo off
REM Windows Build Script using GCC (MinGW)

REM Command Explanation:
REM gcc                 : The compiler
REM main.c              : Main entry point
REM db/db.c    : Database connection logic
REM db/migrations.c  : Migration logic
REM sqlite/sqlite3.c    : SQLite implementation
REM -o program.exe      : Output executable name
REM -I.\sqlite          : Include path for header files (using backslash for Windows paths, though forward slash often works too)

gcc main.c db/db.c db/migrations.c sqlite/sqlite3.c -o program.exe -I.\sqlite

IF %ERRORLEVEL% EQU 0 (
    echo Compilation successful.
    echo Run "program.exe up" to apply migrations.
) ELSE (
    echo Compilation failed.
)
pause