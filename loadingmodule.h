#ifndef LOADINGMODULE_H
#define LOADINGMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class LoadingModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit LoadingModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString FreeCase(QString data);
    QString SelectWorkCase(QString data);
    QString Reception(QString data);
    QString CaseList(QString data);
    QString SelectReadCase(QString data);
    QString SelectReadBid(QString data);
signals:
    void finish(QString result);
};

#endif // LOADINGMODULE_H
