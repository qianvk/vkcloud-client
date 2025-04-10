#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <singleton.h>

#include <QSqlDatabase>

enum class DatabaseType {
    kChatDB = 1,
};

class DatabaseManager : public Singleton<DatabaseManager>
{
public:
    DatabaseManager();
    bool InitChatDb();
    QSqlDatabase& GetChatDb() { return chat_db_; }
    bool ExecQuery(QSqlQuery &query);

    bool ExecQuery(DatabaseType db, const QString& query_str);

private:
    QSqlDatabase chat_db_{QSqlDatabase::addDatabase("QSQLITE")};
};

#endif // DATABASEMANAGER_H
