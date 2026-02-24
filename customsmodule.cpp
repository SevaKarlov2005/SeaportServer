#include "customsmodule.h"

CustomsModule::CustomsModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void CustomsModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = FreeBid(data.sliced(2));
    else if (data.sliced(0, 2) == "01")
        result = SelectWorkBid(data.sliced(2));
    else if (data.sliced(0, 2) == "02")
        result = Refusal(data.sliced(2));
    else if (data.sliced(0, 2) == "03")
        result = BidList(data.sliced(2));
    else if (data.sliced(0, 2) == "04")
        result = SelectReadBid(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString CustomsModule::FreeBid(QString data)
{
    // Результат работы
    QString message;

    // Снятие заявки с занятости
    mutex->lock();
    PGresult* result = manager->UpdateBid(1, data);
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        message = "1";

        PQclear(result);
    }

    return message;
}

QString CustomsModule::SelectWorkBid(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    int found;

    // Установка заявки в обработку
    mutex->lock();

    PGresult* result = manager->SelectBid(1, data_list[1]);

    if (result)
    {
        // Количество найденных заявок
        found = PQntuples(result);

        // Если заявка есть
        if (found > 0)
        {
            // Сохраняем её статус для последующего восстановления
            message = PQgetvalue(result, 0, 32);

            // Если заявка не занята
            if (message == "Направлена в таможню")
            {
                // Занимаем её
                PQclear(result);
                result = manager->UpdateBid(0, "Обрабатывается в таможне\v" + data_list[0] + '\v' + data_list[1]);
            }
        }
    }

    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        PQclear(result);

        if (found == 0)
            message = "\x00";
        else
        {
            if (message != "Направлена в таможню")
                message = "\x1A";
        }
    }

    return message;
}

QString CustomsModule::Refusal(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Снятие заявки с занятости и установка комментария
    mutex->lock();

    PGresult* result = manager->UpdateBid(3, data_list[0] + '\v' + data_list[2]);

    if (result)
    {
        PQclear(result);

        result = manager->UpdateBid(1, data_list[1] + '\v' + data_list[2]);
    }


    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        message = "1";

        PQclear(result);
    }

    return message;
}

QString CustomsModule::BidList(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');
    int page = data_list[2].toInt();
    int limit = data_list[3].toInt();

    // Номер первой запрашиваемой записи
    int minimum_tuples = (page - 1) * limit + 1;

    // Результат работы
    QString message;
    int real_tuples;

    // Получение записей
    mutex->lock();

    // Определение количества реальных записей
    PGresult* result = manager->SelectBid(3, data_list.sliced(4).join('\v'));

    // Если запрос произошёл без ошибок
    if (result)
    {
        // Сколько записей есть на самом деле
        real_tuples = QString(PQgetvalue(result, 0, 0)).toInt();
        PQclear(result);

        // Если запрашиваемая запись больше количества имеющихся
        if (real_tuples < minimum_tuples)
        {
            if (real_tuples == 0) // Если записей нет
                minimum_tuples = 1;
            else if (real_tuples % limit != 0) // Если страницы не заполнены целиком
                minimum_tuples = (real_tuples / limit) * limit + 1;
            else // Если страницы заполнены полностью
                minimum_tuples = ((real_tuples / limit) - 1) * limit + 1;
        }

        // Получение заявок
        result = manager->SelectBid(2, data_list.sliced(0, 2).join('\v') + '\v' + QString::number(limit) +
                                           '\v' + QString::number(minimum_tuples - 1) + '\v' + data_list.sliced(4).join('\v'));
    }

    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        if (PQntuples(result) == 0)
            message = "\x00";
        else
        {
            QStringList list;

            for (int i = 0; i < PQntuples(result); i++)
            {
                list.append(QString(PQgetvalue(result, i, 0)) + '\v' + QString(PQgetvalue(result, i, 1)) + '\v' +
                            QString(PQgetvalue(result, i, 2)) + '\v' + QString(PQgetvalue(result, i, 3)));
            }

            int select_page = (minimum_tuples / limit) + ((minimum_tuples % limit) != 0);
            int all_page = ((real_tuples / limit) + ((real_tuples % limit) != 0));

            // Соединение записей, отображаемой страницы и 1, если дальше есть страницы
            message = list.join('\v') + '\v' + QString::number(select_page) + '\v' + QString::number(select_page < all_page);
        }

        PQclear(result);
    }

    return message;
}

QString CustomsModule::SelectReadBid(QString data)
{
    // Результат работы
    QString message;

    // Получение заявки по идентификатору
    mutex->lock();
    PGresult* result = manager->SelectBid(1, data);
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        // Заявка не найдена
        if (PQntuples(result) == 0 ||
            (QString(PQgetvalue(result, 0, 32)) != "Направлена в таможню" && QString(PQgetvalue(result, 0, 32)) != "Обрабатывается в таможне"))
            message = "\x00";
        else
        {
            // Упаковка данных
            QStringList list;
            int counter = PQnfields(result);

            for (int i = 0; i < counter; i++)
            {
                if (PQgetisnull(result, 0, i) == 1)
                    list.append("");
                else
                    list.append(PQgetvalue(result, 0, i));
            }

            message = list.join('\v');
        }

        PQclear(result);
    }

    return message;
}
