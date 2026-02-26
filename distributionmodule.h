#ifndef DISTRIBUTIONMODULE_H
#define DISTRIBUTIONMODULE_H

#include <QObject>
#include <QMutex>
#include "databasemanager.h"

class DistributionModule : public QObject
{
    Q_OBJECT
private:
    DatabaseManager* manager;
    QMutex* mutex;
public:
    explicit DistributionModule(DatabaseManager* manager, QMutex* mutex, QObject *parent = nullptr);
    void ChooseAction(QString data);
    QString FreeBid(QString data);
    QString SelectWorkBid(QString data);
    QString SuggestDistribution(QString data);
    QString CheckDistribution(QString data);
    QString Refusal(QString data);
    QString Reception(QString data);
    QString FreeCases(QString data);
    QString NominatedCaseList(QString data);
    QString AssignCase(QString data);
    QString ReturnCase(QString data);
    QString BidList(QString data);
    QString CaseList(QString data);
    QString SelectReadBid(QString data);
    QString SelectReadCase(QString data);
    QString CheckCaseAddContents(QString data);
    QString AddCase(QString data);
    QString FreeCase(QString data);
    QString SelectUpdateCase(QString data);
    QString CheckCaseUpdateContents(QString data);
    QString UpdateCase(QString data);
    QString DeleteCaseByOptions(QString data);
    QString DeleteCaseByBid(QString data);
signals:
    void finish(QString result);
};

#endif // DISTRIBUTIONMODULE_H
