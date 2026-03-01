#include "planningmodule.h"

#include <QMap>

PlanningModule::PlanningModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void PlanningModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = FreeCase(data.sliced(2));
    else if (data.sliced(0, 2) == "01")
        result = SelectWorkCase(data.sliced(2));
    else if (data.sliced(0, 2) == "02")
        result = SuggestPlan(data.sliced(2));
    else if (data.sliced(0, 2) == "03")
        result = CheckPlan(data.sliced(2));
    else if (data.sliced(0, 2) == "04")
        result = Reception(data.sliced(2));
    else if (data.sliced(0, 2) == "05")
        result = FreeShip(data.sliced(2));
    else if (data.sliced(0, 2) == "06")
        result = AssignShip(data.sliced(2));
    else if (data.sliced(0, 2) == "07")
        result = CaseList(data.sliced(2));
    else if (data.sliced(0, 2) == "08")
        result = ShipList(data.sliced(2));
    else if (data.sliced(0, 2) == "09")
        result = SelectReadCase(data.sliced(2));
    else if (data.sliced(0, 2) == "10")
        result = SelectReadShip(data.sliced(2));
    else if (data.sliced(0, 2) == "11")
        result = CheckShipAddContents(data.sliced(2));
    else if (data.sliced(0, 2) == "12")
        result = AddShip(data.sliced(2));
    else if (data.sliced(0, 2) == "13")
        result = SelectUpdateShip(data.sliced(2));
    else if (data.sliced(0, 2) == "14")
        result = CheckShipUpdateContents(data.sliced(2));
    else if (data.sliced(0, 2) == "15")
        result = UpdateShip(data.sliced(2));
    else if (data.sliced(0, 2) == "16")
        result = DeleteShip(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString PlanningModule::FreeCase(QString data)
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

QString PlanningModule::SelectWorkCase(QString data)
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

            if (message == "Направлен диспетчеру")
            {
                PQclear(result);
                result = manager->UpdateCase(0, "Обрабатывается диспетчером\v" + data);
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
            if (message != "Направлен диспетчеру")
                message = "\x1A";
        }
    }

    return message;
}

QString PlanningModule::SuggestPlan(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message = "";
    bool is_case = true;
    bool is_bid = true;
    bool is_find = true;

    bool placesError = false;

    // Таблица тип контейнера -> тип морского судна
    QMap<QString, QString> table;

    table.insert("R0", "RE");
    table.insert("R1", "RE");
    table.insert("R2", "RE");
    table.insert("R3", "RE");
    table.insert("UT", "UT");
    table.insert("TN", "TN");
    table.insert("HR", "HR");
    table.insert("B0", "BU");
    table.insert("B1", "BU");
    table.insert("B3", "BU");
    table.insert("B4", "BU");
    table.insert("B5", "BU");
    table.insert("B6", "BU");
    table.insert("V0", "VH");
    table.insert("V2", "VH");
    table.insert("V4", "VH");
    table.insert("PL", "PL");
    table.insert("S0", "PL");
    table.insert("S1", "PL");
    table.insert("S2", "PL");
    table.insert("G0", "GP\vSN");
    table.insert("G1", "GP\vSN");
    table.insert("G2", "GP\vSN");
    table.insert("G3", "GP\vSN");

    // Составление плана погрузки
    mutex->lock();

    PGresult* result = manager->SelectCase(3, data_list.sliced(0, 4).join('\v') + "\v%");
    PGresult* places = NULL;

    if (result)
    {
        is_case = PQntuples(result) > 0;

        if (is_case)
        {
            QString bid_number = PQgetvalue(result, 0, 15);
            QString container_type = PQgetvalue(result, 0, 7);
            int gross = qRound(QString(PQgetvalue(result, 0, 8)).toDouble() * 1000);
            PQclear(result);

            result = manager->SelectBid(1, bid_number);

            if (result)
            {
                is_bid = PQntuples(result) > 0;

                if (is_bid)
                {
                    QString country = PQgetvalue(result, 0, 28);
                    PQclear(result);

                    result = manager->SelectShip(3, country + "\v%\v" + table[container_type]);

                    if (result)
                    {
                        is_find = PQntuples(result) > 0;

                        if (is_find)
                        {
                            int counter = PQntuples(result);
                            QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

                            for (int i = 0; i < counter; i++)
                            {
                                if ((QString(PQgetvalue(result, i, 5)) == "Свободно" || QString(PQgetvalue(result, i, 5)) == "Погрузка") &&
                                    (int)qRound(QString(PQgetvalue(result, i, 7)).toDouble() * 1000) + gross <= (long long)qRound(QString(PQgetvalue(result, i, 6)).toDouble() * 1000))
                                {
                                    places = manager->SelectShip(4, PQgetvalue(result, i, 1));

                                    if (!places)
                                    {
                                        placesError = true;
                                    }
                                    else
                                    {
                                        if (PQntuples(places) < 260)
                                        {
                                            for (int j = 0; j < 26; j++)
                                            {
                                                for (int g = 0; g < 10; g++)
                                                {
                                                    bool is_in = false;

                                                    for (int r = 0; r < PQntuples(places); r++)
                                                    {
                                                        if ((alphabet[j] + QString::number(g).rightJustified(2, '0')) == PQgetvalue(places, r, 0))
                                                        {
                                                            is_in = true;
                                                            break;
                                                        }
                                                    }

                                                    if (!is_in)
                                                    {
                                                        message = country + '\v' + PQgetvalue(result, i, 1) + '\v' +
                                                                  alphabet[j] + QString::number(g).rightJustified(2, '0');

                                                        for (int u = 0; u < PQntuples(places); u++)
                                                            message += ('\v' + QString(PQgetvalue(places, u, 0)));

                                                        break;
                                                    }
                                                }

                                                if (message != "")
                                                    break;
                                            }
                                        }

                                        PQclear(places);
                                        places = nullptr;
                                    }
                                }

                                if (message != "")
                                    break;
                            }

                            is_find = message != "";

                            if (is_find)
                            {
                                QString IMO_number = message.split('\v')[1];

                                PQclear(result);

                                result = manager->UpdateShip(0, "Обрабатывается\v" + IMO_number + '\v' + data_list[4]);
                            }
                        }
                    }
                }
            }
        }
    }

    mutex->unlock();

    // Проверка результата
    if (!result || placesError)
    {
        message = "\x18";

        if (result)
            PQclear(result);

        if (places)
            PQclear(places);
    }
    else if (!is_case || !is_bid)
    {
        message = "\x00";

        PQclear(result);
    }
    else
    {
        if (!is_find)
            message = "\x00";

        PQclear(result);
    }

    return message;
}

QString PlanningModule::CheckPlan(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message = "";
    bool is_right = true;

    // Проверка плана погрузки
    mutex->lock();

    PGresult* result = manager->SelectCase(3, data_list.sliced(0, 4).join('\v') + "\v%");

    if (result)
    {
        is_right = PQntuples(result) > 0;

        if (is_right)
        {
            int gross = qRound(QString(PQgetvalue(result, 0, 8)).toDouble() * 1000);

            PQclear(result);
            result = manager->SelectShip(2, data_list[4]);

            if (result)
            {
                long long payload = qRound(QString(PQgetvalue(result, 0, 6)).toDouble() * 1000);
                long long load = qRound(QString(PQgetvalue(result, 0, 7)).toDouble() * 1000);

                is_right = (load + gross) <= payload;

                if (is_right)
                {
                    PQclear(result);

                    result = manager->SelectShip(4, data_list[4]);

                    if (result)
                    {
                        int counter = PQntuples(result);

                        for (int i = 0; i < counter; i++)
                        {
                            if (data_list[5] == PQgetvalue(result, i, 0))
                            {
                                is_right = false;
                                break;
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
        if (!is_right)
            message = "2";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString PlanningModule::Reception(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Снятие контейнера и морского судна с занятости
    mutex->lock();

    // Снятие контейнера с занятости
    PGresult* result = manager->UpdateCase(1, "Направлен стивидору\v" + data_list.sliced(0, 4).join('\v'));

    if (result)
    {
        PQclear(result);

        result = manager->UpdateCase(4, data_list.sliced(4, 2).join('\v') + '\v' + data_list.sliced(0, 4).join('\v'));

        if (result)
        {
            PQclear(result);

            result = manager->SelectCase(3, data_list.sliced(0, 4).join('\v') + "\v%");

            if (result)
            {
                int gross = qRound(QString(PQgetvalue(result, 0, 8)).toDouble() * 1000);

                PQclear(result);

                result = manager->SelectShip(2, data_list[4]);

                if (result)
                {
                    long long load = qRound(QString(PQgetvalue(result, 0, 7)).toDouble() * 1000);

                    PQclear(result);

                    result = manager->UpdateShip(2, QString::number((load + gross) / 1000.0, 'f', 3) + '\v' + data_list[4]);

                    if (result)
                    {
                        PQclear(result);

                        result = manager->UpdateShip(1, "Погрузка\v" + data_list[4]);
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

QString PlanningModule::FreeShip(QString data)
{
    // Результат работы
    QString message;

    // Снятие морского судна с занятости
    mutex->lock();
    PGresult* result = manager->UpdateShip(1, data);
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

QString PlanningModule::AssignShip(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    QStringList list;
    bool is_busy = false;
    bool is_find = true;
    bool is_wrong = false;

    // Таблица тип контейнера -> тип морского судна
    QMap<QString, QString> table;

    table.insert("R0", "RE");
    table.insert("R1", "RE");
    table.insert("R2", "RE");
    table.insert("R3", "RE");
    table.insert("UT", "UT");
    table.insert("TN", "TN");
    table.insert("HR", "HR");
    table.insert("B0", "BU");
    table.insert("B1", "BU");
    table.insert("B3", "BU");
    table.insert("B4", "BU");
    table.insert("B5", "BU");
    table.insert("B6", "BU");
    table.insert("V0", "VH");
    table.insert("V2", "VH");
    table.insert("V4", "VH");
    table.insert("PL", "PL");
    table.insert("S0", "PL");
    table.insert("S1", "PL");
    table.insert("S2", "PL");
    table.insert("G0", "GP\vSN");
    table.insert("G1", "GP\vSN");
    table.insert("G2", "GP\vSN");
    table.insert("G3", "GP\vSN");

    // Назначение морского судна
    mutex->lock();

    PGresult* result = manager->SelectCase(3, data_list.sliced(0, 4).join('\v') + "\v%");

    if (result)
    {
        QStringList ship_type = table[PQgetvalue(result, 0, 7)].split('\v');

        PQclear(result);

        result = manager->SelectShip(2, data_list[4]);

        if (result)
        {
            is_find = PQntuples(result) > 0;

            if (is_find)
            {
                is_busy = QString(PQgetvalue(result, 0, 5)) != "Свободен" && QString(PQgetvalue(result, 0, 5)) != "Погрузка";

                if (!is_busy)
                {
                    if (ship_type.length() == 1)
                        is_wrong = ship_type[0] != PQgetvalue(result, 0, 2);
                    else
                        is_wrong = ship_type[0] != PQgetvalue(result, 0, 2) && ship_type[1] != PQgetvalue(result, 0, 2);

                    is_wrong = is_wrong && !(data_list[8] == PQgetvalue(result, 0, 4));

                    if (!is_wrong)
                    {
                        PQclear(result);

                        // Снятие морского судна с обработки
                        result = manager->UpdateShip(1, data_list[6] + '\v' + data_list[5]);

                        if (result)
                        {
                            PQclear(result);

                            // Назначение морского судна в обработку
                            result = manager->UpdateShip(0, "Обрабатывается\v" + data_list[7] + '\v' + data_list[4]);

                            if (result)
                            {
                                PQclear(result);

                                result = manager->SelectShip(4, data_list[4]);

                                if (result)
                                {
                                    int counter = PQntuples(result);

                                    for (int i = 0; i < counter; i++)
                                    {
                                        list.append(PQgetvalue(result, i, 0));
                                    }
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
        if (!is_find)
            message = "\x00";
        else if (is_busy)
            message = "\x1A";
        else if (is_wrong)
            message = "2";
        else
            message = list.join('\v');

        PQclear(result);
    }

    return message;
}

QString PlanningModule::CaseList(QString data)
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

QString PlanningModule::ShipList(QString data)
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
    PGresult* result = manager->SelectShip(1, data_list.sliced(4).join('\v'));

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

        // Получение морских судов
        result = manager->SelectShip(0, data_list.sliced(0, 2).join('\v') + '\v' + QString::number(limit) +
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
                            QString(PQgetvalue(result, i, 2)));
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

QString PlanningModule::SelectReadCase(QString data)
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
            (QString(PQgetvalue(result, 0, 13)) != "Направлен диспетчеру" && QString(PQgetvalue(result, 0, 13)) != "Обрабатывается диспетчером"))
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

QString PlanningModule::SelectReadShip(QString data)
{
    // Результат работы
    QString message;

    // Получение морского судна по IMO номеру
    mutex->lock();
    PGresult* result = manager->SelectShip(2, data);
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        // Морское судно не найдено
        if (PQntuples(result) == 0)
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

QString PlanningModule::CheckShipAddContents(QString data)
{
    // Результат работы
    QString message;

    // Проверка существования морского судна
    mutex->lock();
    PGresult* result = manager->SelectShip(2, data);
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        // Контейнер найден
        if (PQntuples(result) > 0)
            message = "2";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString PlanningModule::AddShip(QString data)
{
    // Результат работы
    QString message;

    // Добавление контейнера
    mutex->lock();
    PGresult* result = manager->AddShip(data);
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

QString PlanningModule::SelectUpdateShip(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    QStringList list;
    int found;

    // Установка морского судна в обработку
    mutex->lock();

    PGresult* result = manager->SelectShip(2, data_list[1]);

    if (result)
    {
        found = PQntuples(result);

        if (found > 0)
        {
            message = PQgetvalue(result, 0, 5);

            if (message == "Свободно" || message == "Погрузка")
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
                result = manager->UpdateShip(0, "Обрабатывается\v" + data);
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
            if (message != "Свободно" && message != "Погрузка")
                message = "\x1A";
            else
                message = list.join('\v');
        }
    }

    return message;
}

QString PlanningModule::CheckShipUpdateContents(QString data)
{
    // Результат работы
    QString message;

    // Проверка существования контейнера
    mutex->lock();
    PGresult* result = manager->SelectShip(5, data);
    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        if (PQntuples(result) > 0)
            message = "2";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString PlanningModule::UpdateShip(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Изменение морского судна
    mutex->lock();

    PGresult* result = manager->UpdateShip(3, data_list.sliced(1, 8).join('\v'));

    if (result)
    {
        PQclear(result);

        result = manager->UpdateShip(1, data_list[0] + '\v' + data_list[1]);
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

QString PlanningModule::DeleteShip(QString data)
{
    // Результат работы
    QString message;
    bool is_find = true;
    bool is_busy = false;

    // Удаление морского судна по IMO номеру
    mutex->lock();

    PGresult* result = manager->SelectShip(2, data);

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            is_busy = QString(PQgetvalue(result, 0, 5)) != "Свободно" && QString(PQgetvalue(result, 0, 5)) != "Уплыло";

            if (!is_busy)
            {
                PQclear(result);

                result = manager->DeleteShip(data);
            }
        }
    }

    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        if (!is_find)
            message = "\x00";
        else if (is_busy)
            message = "\x1A";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}
