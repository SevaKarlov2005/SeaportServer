#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QThread>
#include <QList>
#include <QQueue>
#include <QTimer>
#include <openssl/evp.h>
#include "databasemanager.h"
#include "authorizationmodule.h"
#include "administrationmodule.h"
#include "customsmodule.h"
#include "distributionmodule.h"
#include "planningmodule.h"
#include "loadingmodule.h"

class TCPServer : public QObject
{
    Q_OBJECT
private:
    unsigned char key[32];
    unsigned char iv[16];
    EVP_CIPHER_CTX* ctx;
    DatabaseManager* manager;
    QMutex mutex;
    QThread threads[6];
    AuthorizationModule* auth;
    AdministrationModule* adm;
    CustomsModule* cus;
    DistributionModule* dis;
    PlanningModule* pln;
    LoadingModule* ldn;
    QTimer timer;
    QList<QTcpSocket*> incoming_list;
    QList<std::pair<QTcpSocket*, int>> speaking_list;
    QQueue<std::pair<QTcpSocket*, QString>> queues[6];
    QTcpSocket* workers[6];
    bool is_lock[6];
    bool is_come[6];
    QTcpServer server;
public:
    explicit TCPServer(const char* port, QObject *parent = nullptr);
    ~TCPServer();
    QString Encryption(QString data);
    QString Decryption(QString data);
    QByteArray Marking(QString data);
    void HandleClients();
signals:
    void AuthorizationRequest(QString message);
    void AdministrationRequest(QString message);
    void CustomsRequest(QString message);
    void DistributionRequest(QString message);
    void PlanningRequest(QString message);
    void LoadingRequest(QString message);
};

#endif // TCPSERVER_H
