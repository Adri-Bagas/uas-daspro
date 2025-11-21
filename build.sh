# COMPILATION COMMAND EXPLANATION:
# gcc             : The compiler
# main.c          : main application
# db/db.c         : modular database wrapper
# db/migrations.c : migration logic (NEW)
# sqlite/sqlite3.c: The SQLite amalgamation source code (MUST be compiled!)
#
# -o program      : The output filename
# -I./sqlite      : Adds the './sqlite' folder to the include path. 
#                   This allows "#include <sqlite3.h>" to work even though 
#                   the file is in a subfolder.
# -lpthread -ldl  : Required system libraries for SQLite on Linux/Mac.

gcc main.c db/db.c db/migrations.c sqlite/sqlite3.c -o program -I./sqlite -lpthread -ldl
echo "Compilation finished. Run ./program to test."