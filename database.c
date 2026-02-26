#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

#define DB_FILE "app.db"

static sqlite3 *db = NULL;
static int current_user_id = 0;

/* ── helpers ─────────────────────────────────────────────────────────────── */
static void fatal(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static void check_rc(int rc, const char *msg) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        fprintf(stderr, "SQLite error (%s): %s\n", msg, sqlite3_errmsg(db));
        exit(1);
    }
}

/* ── schema ──────────────────────────────────────────────────────────────── */
static void init_schema(void) {
    const char *sql =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS user ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    password TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS note ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    owner_id INTEGER NOT NULL REFERENCES user(id),"
        "    content TEXT NOT NULL,"
        "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    check_rc(rc, "init schema");
}

/* ── public DB lifecycle ─────────────────────────────────────────────────── */
void open_db(void) {
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) fatal("cannot open database");
    init_schema();
}

void close_db(void) {
    if (db) sqlite3_close(db);
    db = NULL;
}

/* Aliases used by main.c */
int init_database(void) {
    open_db();
    return (db != NULL) ? 1 : 0;
}

void close_database(void) {
    close_db();
}

/* ── user operations ─────────────────────────────────────────────────────── */
static void register_user(void) {
    char password[128];

    printf("Register\n");
    printf("Password: ");
    if (!fgets(password, sizeof(password), stdin)) return;
    password[strcspn(password, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO user(password) VALUES(?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert user");

    sqlite3_bind_text(stmt, 1, password, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
        printf("Registration failed.\n");
    else
        printf("Registered successfully.\n");

    sqlite3_finalize(stmt);
}

static int do_login(void) {
    char password[128];

    printf("Login\n");
    printf("Password: ");
    if (!fgets(password, sizeof(password), stdin)) return 0;
    password[strcspn(password, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, password FROM user";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare select user");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *stored = sqlite3_column_text(stmt, 1);
        if (strcmp((const char *)stored, password) == 0) {
            current_user_id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    printf("Login failed.\n");
    return 0;
}

/* ── note operations ─────────────────────────────────────────────────────── */
static void list_all_notes(void) {
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id, content, updated_at "
        "FROM note "
        "ORDER BY updated_at DESC";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare select notes");

    printf("\n===== All Notes =====\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *content = (const char *)sqlite3_column_text(stmt, 1);
        const char *updated = (const char *)sqlite3_column_text(stmt, 2);
        printf("%3d | %s | %s\n", id, content, updated);
    }

    sqlite3_finalize(stmt);
}

static void add_note(void) {
    char buf[1024];

    printf("Content: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO note(owner_id, content) VALUES(?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert note");

    sqlite3_bind_int(stmt, 1, current_user_id);
    sqlite3_bind_text(stmt, 2, buf, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("Failed to add note.\n");

    sqlite3_finalize(stmt);
}

static void edit_note(void) {
    int note_id;
    char buf[1024];

    printf("Note ID to edit: ");
    if (scanf("%d", &note_id) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    sqlite3_stmt *stmt;
    const char *checksql = "SELECT owner_id FROM note WHERE id = ?";

    int rc = sqlite3_prepare_v2(db, checksql, -1, &stmt, NULL);
    check_rc(rc, "prepare check owner");

    sqlite3_bind_int(stmt, 1, note_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW || sqlite3_column_int(stmt, 0) != current_user_id) {
        printf("You don't own that note.\n");
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    printf("New content: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    const char *updatesql =
        "UPDATE note SET content = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?";

    rc = sqlite3_prepare_v2(db, updatesql, -1, &stmt, NULL);
    check_rc(rc, "prepare update note");

    sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, note_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("Failed to edit note.\n");

    sqlite3_finalize(stmt);
}

/* ── user menu (called from main.c via user_login()) ─────────────────────── */
static void user_menu(void) {
    int done = 0;

    while (!done) {
        list_all_notes();
        printf("\nLogged in (id=%d)\n", current_user_id);
        printf("1) Add note\n");
        printf("2) Edit note (own only)\n");
        printf("3) Logout\n");
        printf("Choose> ");

        int c;
        if (scanf("%d", &c) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');

        switch (c) {
            case 1: add_note();  break;
            case 2: edit_note(); break;
            case 3: done = 1;    break;
            default: break;
        }
    }

    current_user_id = 0;
}

void user_login(void) {
    if (do_login()) user_menu();
}
