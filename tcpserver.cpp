#include "tcpserver.h"

#include <QCoreApplication>

TCPServer::TCPServer(const char* port, QObject *parent) : QObject{parent}
{
    // Обнуление указателей для корректной работы деструктора
    this->ctx = NULL;
    this->manager = NULL;
    this->auth = NULL;
    this->adm = NULL;
    this->cus = NULL;
    this->dis = NULL;
    this->pln = NULL;
    this->ldn = NULL;

    // Ключ шифрования и вектор инициализации
    memcpy(this->key, "01234567012345670123456701234567", 32);
    memcpy(this->iv, "0000000000000000", 16);

    // Контекст шифрования
    this->ctx = EVP_CIPHER_CTX_new();

    // Подключение базы данных
    this->manager = new DatabaseManager(port);

    // Инициализация рабочих классов
    this->auth = new AuthorizationModule(this->manager, &this->mutex);
    this->adm = new AdministrationModule(this->manager, &this->mutex);
    this->cus = new CustomsModule(this->manager, &this->mutex);
    this->dis = new DistributionModule(this->manager, &this->mutex);
    this->pln = new PlanningModule(this->manager, &this->mutex);
    this->ldn = new LoadingModule(this->manager, &this->mutex);

    // Инициализация флагов
    for (int i = 0; i < 6; i++)
    {
        this->is_lock[i] = false;
        this->is_come[i] = false;
    }

    // Инициализация рабочего массива
    for (int i = 0; i < 6; i++)
    {
        this->workers[i] = NULL;
    }

    // Подключение сигналов к слотам
    connect(this, &TCPServer::AuthorizationRequest, this->auth, &AuthorizationModule::ChooseAction);
    connect(this, &TCPServer::AdministrationRequest, this->adm, &AdministrationModule::ChooseAction);
    connect(this, &TCPServer::CustomsRequest, this->cus, &CustomsModule::ChooseAction);
    connect(this, &TCPServer::DistributionRequest, this->dis, &DistributionModule::ChooseAction);
    connect(this, &TCPServer::PlanningRequest, this->pln, &PlanningModule::ChooseAction);
    connect(this, &TCPServer::LoadingRequest, this->ldn, &LoadingModule::ChooseAction);

    connect(this->auth, &AuthorizationModule::finish, this,
            [this](QString result){ this->workers[0]->write(Marking(result)); this->workers[0]->flush(); this->is_come[0] = true; },
            Qt::QueuedConnection);
    connect(this->adm, &AdministrationModule::finish, this,
            [this](QString result){ this->workers[1]->write(Marking(result)); this->workers[1]->flush(); this->is_come[1] = true; },
            Qt::QueuedConnection);
    connect(this->cus, &CustomsModule::finish, this,
            [this](QString result){ this->workers[2]->write(Marking(result)); this->workers[2]->flush(); this->is_come[2] = true; },
            Qt::QueuedConnection);
    connect(this->dis, &DistributionModule::finish, this,
            [this](QString result){ this->workers[3]->write(Marking(result)); this->workers[3]->flush(); this->is_come[3] = true; },
            Qt::QueuedConnection);
    connect(this->pln, &PlanningModule::finish, this,
            [this](QString result){ this->workers[4]->write(Marking(result)); this->workers[4]->flush(); this->is_come[4] = true; },
            Qt::QueuedConnection);
    connect(this->ldn, &LoadingModule::finish, this,
            [this](QString result){ this->workers[5]->write(Marking(result)); this->workers[5]->flush(); this->is_come[5] = true; },
            Qt::QueuedConnection);

    // Загрузка рабочих классов в потоки
    this->auth->moveToThread(this->threads);
    this->adm->moveToThread(this->threads + 1);
    this->cus->moveToThread(this->threads + 2);
    this->dis->moveToThread(this->threads + 3);
    this->pln->moveToThread(this->threads + 4);
    this->ldn->moveToThread(this->threads + 5);

    // Запуск потоков
    for (int i = 0; i < 6; i++)
        this->threads[i].start();

    // Запуск сервера
    if (!this->server.listen(QHostAddress::Any, 8081))
    {
        qDebug() << "Error creating server";
        QCoreApplication::quit();
    }
    else
    {
        qDebug() << "Successful server creation";

        this->timer.setInterval(50);
        connect(&this->timer, &QTimer::timeout, this, &TCPServer::HandleClients);
        this->timer.start();
    }
}

TCPServer::~TCPServer()
{
    // Закрытие доступа к серверу
    if (this->server.isListening())
        this->server.close();

    // Завершение запущенных потоков
    for (int i = 0; i < 6; i++)
    {
        if (this->threads[i].isRunning())
        {
            this->threads[i].quit();
            this->threads[i].wait();
        }
    }

    // Удаление поступивших соединений
    for (QList<QTcpSocket*>::iterator it = this->incoming_list.begin(); it != this->incoming_list.end(); it++)
        delete *it;

    // Удаление соединений получающих данные
    for (QList<std::pair<QTcpSocket*, int>>::iterator it = this->speaking_list.begin(); it != this->speaking_list.end(); it++)
        delete (*it).first;

    // Очистка рабочих очередей
    for (int i = 0; i < 6; i++)
    {
        while(!this->queues[i].isEmpty())
            delete this->queues[i].dequeue().first;
    }

    // Удаление рабочих подключений
    for (int i = 0; i < 6; i++)
    {
        if (this->workers[i])
            delete this->workers[i];
    }

    // Удаление модуля авторизации
    if (this->auth)
        delete this->auth;

    // Удаление модуля администрирования
    if (this->adm)
        delete this->adm;

    // Удаление таможенного модуля
    if (this->cus)
        delete this->cus;

    // Удаление модуля распределения
    if (this->dis)
        delete this->dis;

    // Удаление модуля планирования
    if (this->pln)
        delete this->pln;

    // Удаление модуля погрузки
    if (this->ldn)
        delete this->ldn;

    // Удаление контекста шифрования
    if (this->ctx)
        EVP_CIPHER_CTX_free(this->ctx);

    // Удаление соединения с базой данных
    if (this->manager)
        delete this->manager;
}

QString TCPServer::Encryption(QString data)
{
    // Размеры шифрования
    int old_length = data.toUtf8().length();
    int new_length;
    int converted = 0;
    int tmp = 0;

    if (old_length % 16 == 0)
        new_length = old_length + 16;
    else
        new_length = old_length + (16 - (old_length) % 16) % 16;

    unsigned char* new_message = new unsigned char[new_length];

    // Шифрование сообщения
    EVP_EncryptInit_ex(this->ctx, EVP_aes_256_cbc(), NULL, this->key, this->iv);
    EVP_EncryptUpdate(this->ctx, new_message, &converted, (unsigned char*)data.toUtf8().data(), old_length);
    EVP_EncryptFinal_ex(this->ctx, new_message + converted, &tmp);

    QString result = QByteArray::fromRawData((char*)new_message, converted + tmp).toBase64();

    // Очистка вспомогательной памяти
    delete[] new_message;

    return result;
}

QString TCPServer::Decryption(QString data)
{
    // Размеры шифрования
    int old_length = QByteArray::fromBase64(data.toUtf8()).length();
    int converted = 0;
    int tmp = 0;

    unsigned char* new_message = new unsigned char[old_length + 16];

    // Расшифровка сообщения
    EVP_DecryptInit_ex(this->ctx, EVP_aes_256_cbc(), NULL, this->key, this->iv);
    EVP_DecryptUpdate(this->ctx, new_message, &converted, (unsigned char*)QByteArray::fromBase64(data.toUtf8()).data(), old_length);
    EVP_DecryptFinal_ex(this->ctx, new_message + converted, &tmp);

    QString result = QString::fromUtf8((char*)new_message, converted + tmp);

    // Очистка вспомогательной памяти
    delete[] new_message;

    return result;
}

QByteArray TCPServer::Marking(QString data)
{
    // Шифровка сообщения
    data = Encryption(data);

    // Добавление заголовка
    int length = data.toUtf8().length();
    QByteArray bytes = (QString::number(length).rightJustified(7, '0') + data).toUtf8();

    return bytes;
}

void TCPServer::HandleClients()
{
    // Подключаем клиентов
    while (this->server.hasPendingConnections())
        this->incoming_list.push_back(this->server.nextPendingConnection());

    // Проверка есть ли сообщения
    for (int i = 0; i < this->incoming_list.size();)
    {
        if (this->incoming_list[i]->bytesAvailable() >= 7)
        {
            this->speaking_list.push_back(std::pair<QTcpSocket*, int>(this->incoming_list[i], this->incoming_list[i]->read(7).trimmed().toInt()));
            this->incoming_list.remove(i);
        }
        else
            i++;
    }

    // Проверка полноты сообщения
    QString message;

    for (int i = 0; i < this->speaking_list.size();)
    {
        if (this->speaking_list[i].first->bytesAvailable() == this->speaking_list[i].second)
        {
            message = Decryption(QString::fromUtf8(this->speaking_list[i].first->read(this->speaking_list[i].second)));

            if (message[0] == 'I')
                this->queues[0].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));
            else if (message[0] == 'A')
                this->queues[1].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));
            else if (message[0] == 'C')
                this->queues[2].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));
            else if (message[0] == 'D')
                this->queues[3].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));
            else if (message[0] == 'P')
                this->queues[4].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));
            else if (message[0] == 'L')
                this->queues[5].enqueue(std::pair<QTcpSocket*, QString>(this->speaking_list[i].first, message.sliced(1)));

            this->speaking_list.remove(i);
        }
        else
            i++;
    }

    // Выполнение работы
    for (int i = 0; i < 6; i++)
    {
        if (!this->is_lock[i])
        {
            if (!this->queues[i].isEmpty())
            {
                std::pair<QTcpSocket*, QString> data = this->queues[i].dequeue();

                this->is_lock[i] = true;
                this->is_come[i] = false;
                this->workers[i] = data.first;

                if (i == 0)
                    emit AuthorizationRequest(data.second);
                else if (i == 1)
                    emit AdministrationRequest(data.second);
                else if (i == 2)
                    emit CustomsRequest(data.second);
                else if (i == 3)
                    emit DistributionRequest(data.second);
                else if (i == 4)
                    emit PlanningRequest(data.second);
                else if (i == 5)
                    emit LoadingRequest(data.second);
            }
        }
        else
        {
            if(this->is_come[i])
            {
                this->is_lock[i] = false;
                this->is_come[i] = false;

                this->workers[i]->disconnectFromHost();
                this->workers[i]->deleteLater();

                this->workers[i] = NULL;
            }
        }
    }
}
