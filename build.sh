#!/bin/bash

gcc db/*.c feat/*.c sqlite/sqlite3.c main.c -o fmcon -I./sqlite -lpthread -ldl
echo "Compilation finished. Run \"./fmcon init\" to test."