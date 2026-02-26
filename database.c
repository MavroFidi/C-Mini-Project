#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define DB_FILE "app.db"

static sqlite3 *db = NULL;
static int current_user_id = 0;

void fatal(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void check_rc(int rc, const char *msg) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        fprintf(stderr, "SQLite error (%s): %s\n", msg, sqlite3_errmsg(db));
        exit(1);
    }
}

void init_db(void) {
    int rc;
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

    rc = sqlite3_exec(db, sql, 0, 0, NULL);
    check_rc(rc, "init schema");
}

void open_db(void) {
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) fatal("cannot open database");
    init_db();
}

void close_db(void) {
    if (db) sqlite3_close(db);
}

void register_user(void) {
    char password[128];

    printf("register\n");
    printf("password: ");
    if (!fgets(password, sizeof(password), stdin)) return;
    password[strcspn(password, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO user(password) VALUES(?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert user");

    sqlite3_bind_text(stmt, 1, password, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("registration failed\n");
    } else {
        printf("registered successfully\n");
    }

    sqlite3_finalize(stmt);
}

int login(void) {
    char password[128];

    printf("login\n");
    printf("password: ");
    if (!fgets(password, sizeof(password), stdin)) return 0;
    password[strcspn(password, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, password FROM user";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare select user");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *stored = sqlite3_column_text(stmt, 1);
        if (strcmp((const char*)stored, password) == 0) {
            current_user_id = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    printf("login failed\n");
    return 0;
}

void list_all_notes(void) {
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id, content, updated_at "
        "FROM note "
        "ORDER BY updated_at DESC";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare select notes");

    printf("\n===== all notes =====\n");

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *content = (const char*)sqlite3_column_text(stmt, 1);
        const char *updated = (const char*)sqlite3_column_text(stmt, 2);

        printf("%3d | %s | %s\n", id, content, updated);
    }

    sqlite3_finalize(stmt);
}

void add_note(void) {
    char buf[1024];

    printf("content: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO note(owner_id,content) VALUES(?,?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    check_rc(rc, "prepare insert note");

    sqlite3_bind_int(stmt, 1, current_user_id);
    sqlite3_bind_text(stmt, 2, buf, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("failed to add note\n");

    sqlite3_finalize(stmt);
}

void edit_note(void) {
    int note_id;
    char buf[1024];

    printf("note id to edit: ");
    if (scanf("%d", &note_id) != 1) { while(getchar()!= '\n'); return; }
    while(getchar()!= '\n');

    sqlite3_stmt *stmt;
    const char *checksql = "SELECT owner_id FROM note WHERE id = ?";

    int rc = sqlite3_prepare_v2(db, checksql, -1, &stmt, NULL);
    check_rc(rc, "prepare check owner");

    sqlite3_bind_int(stmt, 1, note_id);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW || sqlite3_column_int(stmt,0) != current_user_id) {
        printf("you don't own that note\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    printf("new content: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    const char *updatesql =
        "UPDATE note SET content = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?";

    rc = sqlite3_prepare_v2(db, updatesql, -1, &stmt, NULL);
    check_rc(rc, "prepare update note");

    sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, note_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) printf("failed to edit note\n");

    sqlite3_finalize(stmt);
}

void user_menu(void) {
    int done = 0;

    while (!done) {
        list_all_notes();
        printf("\nLogged in (id=%d)\n", current_user_id);
        printf("1) add note\n");
        printf("2) edit note (own only)\n");
        printf("3) logout\n");
        printf("choose> ");

        int c;
        if (scanf("%d", &c) != 1) { while(getchar()!= '\n'); continue; }
        while(getchar()!= '\n');

        switch(c) {
            case 1: add_note(); break;
            case 2: edit_note(); break;
            case 3: done = 1; break;
            default: break;
        }
    }

    current_user_id = 0;
}

int main(void) {
    open_db();

    int running = 1;

    while (running) {
        printf("\n=== menu ===\n");
        printf("1) register\n");
        printf("2) login\n");
        printf("3) quit\n");
        printf("choose> ");

        int c;
        if (scanf("%d", &c) != 1) { while(getchar()!= '\n'); continue; }
        while(getchar()!= '\n');

        switch(c) {
            case 1: register_user(); break;
            case 2:
                if (login()) user_menu();
                break;
            case 3: running = 0; break;
            default: break;
        }
    }

    close_db();
    return 0;
}