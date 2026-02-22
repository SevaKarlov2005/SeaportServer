#include "authorizationmodule.h"

AuthorizationModule::AuthorizationModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void AuthorizationModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = AuthorizationUser(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString AuthorizationModule::AuthorizationUser(QString data)
{
    // Результат работы
    QString message;

    // Поиск пользователя
    mutex->lock();
    PGresult* result = manager->SelectUser(0, data);
    mutex->unlock();

    // Проверка результата поиска
    if (!result)
        message = "\x18";
    else if (PQntuples(result) == 0)
        message = "\x00";
    else
    {
        // Добавление всех полей, кроме последнего
        for (int i = 0; i < (PQnfields(result) - 1); i++)
        {
            if (PQgetisnull(result, 0, i) == 1)
                message += "\v";
            else
            {
                message += PQgetvalue(result, 0, i);
                message += "\v";
            }
        }

        // Добавление последнего поля
        if (PQgetisnull(result, 0, PQnfields(result) - 1) == 0)
            message += PQgetvalue(result, 0, PQnfields(result) - 1);

        PQclear(result);
    }

    return message;
}
