#ifndef PLANNINGMODULE_H
#define PLANNINGMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class PlanningModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit PlanningModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString FreeCase(QString data);
    QString SelectWorkCase(QString data);
    QString SuggestPlan(QString data);
    QString CheckPlan(QString data);
    QString Reception(QString data);
    QString FreeShip(QString data);
    QString AssignShip(QString data);
    QString CaseList(QString data);
    QString ShipList(QString data);
    QString SelectReadCase(QString data);
    QString SelectReadShip(QString data);
    QString CheckShipAddContents(QString data);
    QString AddShip(QString data);
    QString SelectUpdateShip(QString data);
    QString CheckShipUpdateContents(QString data);
    QString UpdateShip(QString data);
    QString DeleteShip(QString data);
signals:
    void finish(QString result);
};

#endif // PLANNINGMODULE_H
