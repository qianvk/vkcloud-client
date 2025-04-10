#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <spdlog/spdlog.h>

DatabaseManager::DatabaseManager() {
    InitChatDb();
}

bool DatabaseManager::InitChatDb()
{
    chat_db_.setDatabaseName("chat.db");
    if (!chat_db_.open()) {
        SPDLOG_ERROR("Error: Unable to open database: {}", chat_db_.lastError().text().toStdString());
        return false;
    }

    QSqlQuery query(chat_db_);
    QString createMessageTable = R"(
            CREATE TABLE IF NOT EXISTS message (
            id INTEGER PRIMARY KEY NOT NULL,
            relate_id INTEGER NOT NULL,
            is_self INTEGER NOT NULL,
            content TEXT,
            type INTEGER NOT NULL,
            status INTEGER NOT NULL DEFAULT 0,
            created DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
            updated DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL
        );
    )";

    if (!query.exec(createMessageTable)) {
        SPDLOG_ERROR("Create table error: {}", query.lastError().text().toStdString());
        return false;
    }


    QStringList indexStatements = {
        R"(CREATE INDEX IF NOT EXISTS idx_relateId_status_created ON message (relate_id, status, created);)",
    };

    for (const QString &stmt : indexStatements) {
        if (!query.exec(stmt)) {
            SPDLOG_ERROR("Create index error:", query.lastError().text().toStdString());
            return false;
        }
    }

    QString createFriendTable = R"(
        CREATE TABLE IF NOT EXISTS friend (
            id         INTEGER PRIMARY KEY NOT NULL,
            nick       TEXT NOT NULL,
            email      TEXT NOT NULL UNIQUE,
            avatar     TEXT NOT NULL
        );
    )";

    if (!query.exec(createFriendTable)) {
        SPDLOG_ERROR("Create table error: {}", query.lastError().text().toStdString());
        return false;
    }

    return true;
}

bool DatabaseManager::ExecQuery(QSqlQuery &query)
{
    if (!query.exec()) {
        SPDLOG_ERROR("ChatDB sql error: {}", query.lastError().text().toStdString());
        return false;
    }
    return true;
}

bool DatabaseManager::ExecQuery(DatabaseType db, const QString &query_str)
{
    switch (db) {
    case DatabaseType::kChatDB: {
        QSqlQuery query(chat_db_);
        if (!query.exec(query_str)) {
            SPDLOG_ERROR("ChatDB sql error: {}", query.lastError().text().toStdString());
            return false;
        }
    } break;
    default:
        break;
    }
    return true;
}
