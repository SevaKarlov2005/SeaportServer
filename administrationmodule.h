#ifndef ADMINISTRATIONMODULE_H
#define ADMINISTRATIONMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class AdministrationModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit AdministrationModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString CheckFeasibility(QString data);
    QString AddBid(QString data);
    QString FreeBid(QString data);
    QString SelectUpdateBid(QString data);
    QString UpdateBid(QString data);
    QString DeleteBid(QString data);
    QString BidList(QString data);
    QString CaseList(QString data);
    QString ShipList(QString data);
    QString SelectReadBid(QString data);
    QString SelectReadCase(QString data);
    QString SelectReadShip(QString data);
    QString AddUser(QString data);
    QString SelectUpdateUser(QString data);
    QString UpdateUser(QString data);
    QString DeleteUser(QString data);
signals:
    void finish(QString result);
};

#endif // ADMINISTRATIONMODULE_H
