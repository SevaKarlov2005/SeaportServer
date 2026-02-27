#include "loadingmodule.h"

LoadingModule::LoadingModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void LoadingModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = FreeCase(data.sliced(2));
    else if (data.sliced(0, 2) == "01")
        result = SelectWorkCase(data.sliced(2));
    else if (data.sliced(0, 2) == "02")
        result = Reception(data.sliced(2));
    else if (data.sliced(0, 2) == "03")
        result = CaseList(data.sliced(2));
    else if (data.sliced(0, 2) == "04")
        result = SelectReadCase(data.sliced(2));
    else if (data.sliced(0, 2) == "05")
        result = SelectReadBid(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString LoadingModule::FreeCase(QString data)
{
    // Результат работы
    QString message;

    // Снятие контейнера с занятости
    mutex->lock();
    PGresult* result = manager->UpdateCase(1, data);
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

QString LoadingModule::SelectWorkCase(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    QStringList list;
    int found;

    // Установка контейнера в обработку
    mutex->lock();

    PGresult* result = manager->SelectCase(3, data_list.sliced(1, 4).join('\v') + "\v%");

    if (result)
    {
        found = PQntuples(result);

        if (found > 0)
        {
            message = PQgetvalue(result, 0, 13);

            if (message == "Направлен стивидору")
            {
                int counter = PQnfields(result);

                for (int i = 0; i < counter; i++)
                {
                    if (PQgetisnull(result, 0, i))
                        list.append("");
                    else
                        list.append(PQgetvalue(result, 0, i));
                }

                PQclear(result);
                result = manager->UpdateCase(0, "Обрабатывается стивидором\v" + data);
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
            if (message != "Направлен стивидору")
                message = "\x1A";
            else
                message = list.join('\v');
        }
    }

    return message;
}

QString LoadingModule::Reception(QString data)
{
    // Комментарий Статус контейнера Контейнер
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Снятие контейнера с занятости и установка комментария
    mutex->lock();
    PGresult* result = manager->UpdateCase(1, data_list.sliced(1, 5).join('\v'));

    if (result)
    {
        PQclear(result);
        result = manager->SelectCase(3, data_list.sliced(2, 4).join('\v') + "\v%");

        if (result)
        {
            QString bid_number = PQgetvalue(result, 0, 15);

            PQclear(result);
            result = manager->UpdateBid(3, data_list[0] + '\v' + bid_number);

            if (result)
            {
                PQclear(result);
                result = manager->SelectCase(11, "Не погружен\v" + bid_number);

                if (result)
                {
                    int wrong = QString(PQgetvalue(result, 0, 0)).toInt();

                    if (wrong > 0)
                    {
                        PQclear(result);
                        result = manager->UpdateBid(1, "Отказ\v" + bid_number);
                    }
                    else
                    {
                        PQclear(result);
                        result = manager->SelectCase(10, bid_number);

                        if (result)
                        {
                            int all = QString(PQgetvalue(result, 0, 0)).toInt();

                            PQclear(result);
                            result = manager->SelectCase(11, "Погружен\v" + bid_number);

                            if (result)
                            {
                                int success = QString(PQgetvalue(result, 0, 0)).toInt();

                                if (success == all)
                                {
                                    PQclear(result);
                                    result = manager->UpdateBid(1, "Принята\v" + bid_number);
                                }
                            }
                        }
                    }
                }
            }
        }
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

QString LoadingModule::CaseList(QString data)
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
    PGresult* result = manager->SelectCase(2, data_list.sliced(4).join('\v'));

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

        // Получение контейнеров
        result = manager->SelectCase(1, data_list.sliced(0, 2).join('\v') + '\v' + QString::number(limit) +
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
                            QString(PQgetvalue(result, i, 2)) + '\v' + QString(PQgetvalue(result, i, 3)) + '\v' +
                            QString(PQgetvalue(result, i, 4)));
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

QString LoadingModule::SelectReadCase(QString data)
{
    // Результат работы
    QString message;

    // Получение контейнера по параметрам
    mutex->lock();
    PGresult* result = manager->SelectCase(3, data + "\v%");
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        // Контейнер не найден
        if (PQntuples(result) == 0 ||
            (QString(PQgetvalue(result, 0, 13)) != "Направлен стивидору" && QString(PQgetvalue(result, 0, 13)) != "Обрабатывается стивидором"))
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

QString LoadingModule::SelectReadBid(QString data)
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
        if (PQntuples(result) == 0 || QString(PQgetvalue(result, 0, 32)) != "Обрабатывается на складе")
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
