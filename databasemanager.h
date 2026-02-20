#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <libpq-fe.h>
#include <QString>

class DatabaseManager
{
private:
    PGconn* connection;
public:
    DatabaseManager(const char* port);
    ~DatabaseManager();
    PGresult* SelectBid(unsigned short mod, QString data);
    PGresult* AddBid(QString data);
    PGresult* UpdateBid(unsigned short mod, QString data);
    PGresult* DeleteBid(QString data);
    PGresult* SelectUser(unsigned short mod, QString data);
    PGresult* AddUser(QString data);
    PGresult* UpdateUser(QString data);
    PGresult* DeleteUser(QString data);
    PGresult* SelectCase(unsigned short mod, QString data);
    PGresult* AddCase(QString data);
    PGresult* UpdateCase(unsigned short mod, QString data);
    PGresult* DeleteCase(unsigned short mod, QString data);
    PGresult* SelectShip(unsigned short mod, QString data);
    PGresult* AddShip(QString data);
    PGresult* UpdateShip(unsigned short mod, QString data);
    PGresult* DeleteShip(QString data);
};

#endif // DATABASEMANAGER_H
