#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(d) _mkdir(d)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(d) mkdir(d, 0777)
#endif


// Config buat enkripsi
#define ENC_FILENAME "data/data.enc"
#define CIPHER_KEY "]a~HGtsy$9xXe,neihado8w071023h hqw9*)&# !)HEA)HShdK%^BAI*WV*&^#!G X(WQU HHOQWH)" 
#define BUFFER_SIZE 2048


/**
 * Ensures the 'data' directory exists.
 */
void ensure_data_dir() {
    // Try to create the directory. 
    // If it exists, this will fail/return -1, which we ignore safely.
    MKDIR("data");
}


/**
 * Encrypts/decrypts a file using XOR with the provided key.
 *
 * This function reads the input file in chunks of size BUFFER_SIZE,
 * encrypts/decrypts each chunk using XOR with the key, and writes
 * the encrypted/decrypted chunk to the output file.
 *
 * @param input_path The path to the input file.
 * @param output_path The path to the output file.
 *
 * @return 1 if the file was successfully processed, 0 otherwise.
 */
int process_file(const char *input_path, const char *output_path) {
    FILE *in = fopen(input_path, "rb");
    FILE *out = fopen(output_path, "wb");
    
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 0; // Error
    }

    const char *key = CIPHER_KEY;
    size_t key_len = strlen(key);
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t key_idx = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            buffer[i] ^= key[key_idx % key_len];
            key_idx++;
        }
        fwrite(buffer, 1, bytes_read, out);
    }

    fclose(in);
    fclose(out);
    return 1; // Success
}

int file_exists(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

// --- CORE FUNCTIONS ---

int openOrCreateDB(const char* db_name, sqlite3** db) {
    // 0. Ensure data directory exists
    ensure_data_dir();

    // 1. Security & Decryption Check
    if (file_exists(db_name)) {
        // Crash recovery: .db exists (app probably crashed last time).
        printf("[SEC] Unencrypted database found. Resuming session...\n");
    } else if (file_exists(ENC_FILENAME)) {
        // Normal start: Decrypt data.enc -> db_name
        printf("[SEC] Decrypting database...\n");
        if (!process_file(ENC_FILENAME, db_name)) {
            fprintf(stderr, "[ERR] Failed to decrypt database.\n");
            return SQLITE_ERROR;
        }
    } else {
        // First run
        printf("[SEC] Creating new secure database...\n");
    }

    // 2. Open SQLite Connection
    int rc = sqlite3_open(db_name, db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        sqlite3_close(*db);
        return rc;
    }

    // 3. Configure Database
    char *err_msg = 0;
    
    // Set Journal Mode to DELETE to avoid .wal artifacts
    sqlite3_exec(*db, "PRAGMA journal_mode = DELETE;", 0, 0, &err_msg);
    if (err_msg) sqlite3_free(err_msg);
    
    // Enable Foreign Keys
    sqlite3_exec(*db, "PRAGMA foreign_keys = ON;", 0, 0, 0);

    printf("Successfully connected to database: %s\n", db_name);
    return SQLITE_OK;
}

void close_db(sqlite3 *db) {
    if (db) {
        // Retrieve the filename associated with this connection before closing
        const char* db_filename = sqlite3_db_filename(db, "main");
        
        // Default fallback updated to include data/ folder
        char filename_copy[256] = "data/data.db"; 
        
        if (db_filename && strlen(db_filename) > 0) {
            strncpy(filename_copy, db_filename, sizeof(filename_copy) - 1);
        }

        // 1. Close connection to release file lock
        sqlite3_close(db);
        
        // 2. Encrypt .db -> .enc
        printf("[SEC] Encrypting database...\n");
        if (process_file(filename_copy, ENC_FILENAME)) {
            // 3. Delete the plain text file
            remove(filename_copy);
            printf("[SEC] Database secured. Goodbye!\n");
        } else {
            fprintf(stderr, "[ERR] Failed to encrypt database! Raw file left for safety.\n");
        }
    }
}