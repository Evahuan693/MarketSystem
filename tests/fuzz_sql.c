#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

int main(void) {
    sqlite3 *db = NULL;
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) return 0;

    size_t cap = 65536;
    unsigned char *buf = malloc(cap);
    if (!buf) { sqlite3_close(db); return 0; }

    size_t n = 0;
    size_t r;
    while ((r = fread(buf + n, 1, cap - n, stdin)) > 0) {
        n += r;
        if (n + 1 >= cap) {
            size_t ncap = cap * 2;
            unsigned char *nb = realloc(buf, ncap);
            if (!nb) break;
            buf = nb;
            cap = ncap;
        }
    }

    if (n == 0) { free(buf); sqlite3_close(db); return 0; }
    buf[n] = '\0';

    char *err = NULL;
    /* Execute the provided SQL; ignore result but keep error text for potential leaks/crashes */
    sqlite3_exec(db, (const char *)buf, NULL, NULL, &err);
    if (err) sqlite3_free(err);

    free(buf);
    sqlite3_close(db);
    return 0;
}
