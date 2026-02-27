#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

#define DB_FILE "app.db"

static sqlite3 *db = NULL;

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
        "    id       INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name     TEXT NOT NULL UNIQUE,"
        "    passcode TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS note ("
        "    id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    owner_name TEXT NOT NULL REFERENCES user(name),"
        "    content    TEXT NOT NULL,"
        "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    check_rc(rc, "init schema");
}

/* ── DB lifecycle ─────────────────────────────────────────────────────────── */
int init_database(void) {
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) { fatal("cannot open database"); return 0; }
    init_schema();
    return (db != NULL) ? 1 : 0;
}

void close_database(void) {
    if (db) sqlite3_close(db);
    db = NULL;
}

/* ── save_user ────────────────────────────────────────────────────────────── */
void save_user(User *user) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO user(name, passcode) VALUES(?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert user");

    sqlite3_bind_text(stmt, 1, user->name,     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user->passcode, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
        printf("Failed to save user \"%s\".\n", user->name);
    else
        printf("User \"%s\" saved with passcode %s.\n", user->name, user->passcode);

    sqlite3_finalize(stmt);
}

/* ── find_user ────────────────────────────────────────────────────────────── */
int find_user(const char *name, const char *passcode, User *out_user) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT name, passcode FROM user WHERE name = ? AND passcode = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare find user");

    sqlite3_bind_text(stmt, 1, name,     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passcode, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        strncpy(out_user->name,     (const char *)sqlite3_column_text(stmt, 0), 50);
        strncpy(out_user->passcode, (const char *)sqlite3_column_text(stmt, 1), 16);
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

/* ── note operations ─────────────────────────────────────────────────────── */
static void list_notes(const char *username) {
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id, content, updated_at FROM note "
        "ORDER BY updated_at DESC";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare list notes");

    printf("\n===== All Notes =====\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id             = sqlite3_column_int(stmt, 0);
        const char *content = (const char *)sqlite3_column_text(stmt, 1);
        const char *updated = (const char *)sqlite3_column_text(stmt, 2);
        printf("%3d | %s | %s\n", id, content, updated);
    }
    sqlite3_finalize(stmt);
}

static void add_note(const char *username) {
    char buf[1024];

    printf("Content: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO note(owner_name, content) VALUES(?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert note");

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, buf,      -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("Failed to add note.\n");

    sqlite3_finalize(stmt);
}

static void edit_note(const char *username) {
    int note_id;
    char buf[1024];

    printf("Note ID to edit: ");
    if (scanf("%d", &note_id) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    sqlite3_stmt *stmt;
    const char *checksql = "SELECT owner_name FROM note WHERE id = ?";

    int rc = sqlite3_prepare_v2(db, checksql, -1, &stmt, NULL);
    check_rc(rc, "prepare check owner");

    sqlite3_bind_int(stmt, 1, note_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW ||
        strcmp((const char *)sqlite3_column_text(stmt, 0), username) != 0) {
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

    sqlite3_bind_text(stmt, 1, buf,      -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,  2, note_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("Failed to edit note.\n");

    sqlite3_finalize(stmt);
}

/* ── notes_menu ──────────────────────────────────────────────────────────── */
void notes_menu(const char *username) {
    int done = 0;

    while (!done) {
        list_notes(username);
        printf("\nLogged in as: %s\n", username);
        printf("1) Add note\n");
        printf("2) Edit note (own only)\n");
        printf("3) Logout\n");
        printf("Choose> ");

        int c;
        if (scanf("%d", &c) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');

        switch (c) {
            case 1: add_note(username);  break;
            case 2: edit_note(username); break;
            case 3: done = 1;            break;
            default: break;
        }
    }
}

/* ── reset_users ─────────────────────────────────────────────────────────── */
void reset_users(void) {
    /* Delete notes first (foreign key), then users, then reset AUTOINCREMENT
       counters so note/user IDs start from 1 again after reset */
    const char *del_notes = "DELETE FROM note;";
    const char *del_users = "DELETE FROM user;";
    const char *rst_seq   = "DELETE FROM sqlite_sequence "
                            "WHERE name='note' OR name='user';";

    int rc = sqlite3_exec(db, del_notes, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQLite error (reset notes): %s\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_exec(db, del_users, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQLite error (reset users): %s\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_exec(db, rst_seq, 0, 0, NULL);
    if (rc != SQLITE_OK)
        fprintf(stderr, "SQLite error (reset sequence): %s\n", sqlite3_errmsg(db));
}
