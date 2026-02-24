#ifndef CUSTOMSMODULE_H
#define CUSTOMSMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class CustomsModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit CustomsModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString FreeBid(QString data);
    QString SelectWorkBid(QString data);
    QString Refusal(QString data);
    QString BidList(QString data);
    QString SelectReadBid(QString data);
signals:
    void finish(QString result);
};

#endif // CUSTOMSMODULE_H
