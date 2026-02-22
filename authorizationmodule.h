#ifndef AUTHORIZATIONMODULE_H
#define AUTHORIZATIONMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class AuthorizationModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit AuthorizationModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString AuthorizationUser(QString data);
signals:
    void finish(QString result);
};

#endif // AUTHORIZATIONMODULE_H
