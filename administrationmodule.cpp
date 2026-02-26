#include "administrationmodule.h"

#include <QDate>

AdministrationModule::AdministrationModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void AdministrationModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = CheckFeasibility(data.sliced(2));
    else if (data.sliced(0, 2) == "01")
        result = AddBid(data.sliced(2));
    else if (data.sliced(0, 2) == "02")
        result = FreeBid(data.sliced(2));
    else if (data.sliced(0, 2) == "03")
        result = SelectUpdateBid(data.sliced(2));
    else if (data.sliced(0, 2) == "04")
        result = UpdateBid(data.sliced(2));
    else if (data.sliced(0, 2) == "05")
        result = DeleteBid(data.sliced(2));
    else if (data.sliced(0, 2) == "06")
        result = BidList(data.sliced(2));
    else if (data.sliced(0, 2) == "07")
        result = CaseList(data.sliced(2));
    else if (data.sliced(0, 2) == "08")
        result = ShipList(data.sliced(2));
    else if (data.sliced(0, 2) == "09")
        result = SelectReadBid(data.sliced(2));
    else if (data.sliced(0, 2) == "10")
        result = SelectReadCase(data.sliced(2));
    else if (data.sliced(0, 2) == "11")
        result = SelectReadShip(data.sliced(2));
    else if (data.sliced(0, 2) == "12")
        result = AddUser(data.sliced(2));
    else if (data.sliced(0, 2) == "13")
        result = SelectUpdateUser(data.sliced(2));
    else if (data.sliced(0, 2) == "14")
        result = UpdateUser(data.sliced(2));
    else if (data.sliced(0, 2) == "15")
        result = DeleteUser(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString AdministrationModule::CheckFeasibility(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Первые две цифры ТН ВЭД кода
    int code = data_list[0].sliced(0, 2).toInt();

    // Выбор группы контейнеров
    QString case_group = "";

    if ((code >= 1 && code <= 8) || code == 15 || (code >= 16 && code <= 21) || code == 30)
        case_group = "R0\vR1\vR2\vR3";
    else if (code == 22 || code == 27 || (code >= 28 && code <= 29) || (code >= 31 && code <= 38))
        case_group = "UT\vTN\vHR";
    else if ((code >= 9 && code <= 10) || code == 23 || (code >= 25 && code <= 26))
        case_group = "B0\vB1\vB3\vB4\vB5\vB6";
    else if (code >= 11 && code <= 14)
        case_group = "V0\vV2\vV4";
    else if ((code >= 44 && code <= 46) || (code >= 72 && code <= 83) || (code >= 86 && code <= 89))
        case_group = "PL\vS0\vS1\vS2";
    else
        case_group = "G0\vG1\vG2\vG3";

    // Поиск контейнеров
    mutex->lock();
    PGresult* result1 = manager->SelectCase(0, case_group);
    mutex->unlock();

    // Выбор группы морских судов
    QString ship_group = "";

    if ((code >= 2 && code <= 8) || code == 15 || (code >= 16 && code <= 21) || code == 30)
        ship_group = "RE";
    else if (code == 22 || code == 27 || (code >= 28 && code <= 29) || (code >= 31 && code <= 38))
        ship_group = "UT\vTN\vHR";
    else if ((code >= 9 && code <= 10) || code == 23 || (code >= 25 && code <= 26))
        ship_group = "BU";
    else if ((code >= 44 && code <= 46) || (code >= 72 && code <= 83) || (code >= 86 && code <= 89))
        ship_group = "PL";
    else if (code == 1)
        ship_group = "VH";
    else
        ship_group = "GP\vSN";

    // Поиск морских судов
    mutex->lock();
    PGresult* result2 = manager->SelectShip(3, data_list[1] + "\v%\v" + ship_group);
    mutex->unlock();

    // Проверка результата поиска
    if (!result1 || !result2)
    {
        message = "\x18";

        if (result1)
            PQclear(result1);

        if (result2)
            PQclear(result2);
    }
    else if (PQntuples(result1) > 0 && PQntuples(result2) > 0)
    {
        message = "1";

        PQclear(result1);
        PQclear(result2);
    }
    else
    {
        message = "\x00";

        PQclear(result1);
        PQclear(result2);
    }

    return message;
}

QString AdministrationModule::AddBid(QString data)
{
    // Результат работы
    QString message;

    // Добавление заявки
    mutex->lock();

    // Получаем максимальный идентификатор
    PGresult* result = manager->SelectBid(0, "");

    // Добавляем заявку
    if (result)
    {
        // Номер заявки
        QString number = "%1-" + QString::number(QDate::currentDate().year() % 100) + "/ОМиДР";
        QString maximum = (PQgetisnull(result, 0, 0) == 1) ? "00000" : QString::number((QString(PQgetvalue(result, 0, 0)).toInt() + 1) % 100000);

        maximum = maximum.rightJustified(5, '0');
        number = number.arg(maximum);

        PQclear(result);

        // Дата добавления
        QString cur_date = QDate::currentDate().toString("yyyy-MM-dd");

        result = manager->AddBid(number + "\v" + data + "\v" + cur_date + "\v" + cur_date);
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

QString AdministrationModule::FreeBid(QString data)
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

QString AdministrationModule::SelectUpdateBid(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    QStringList list;
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
            if (message == "Направлена в таможню" || message == "Направлена на склад")
            {
                int counter = PQnfields(result);

                for (int i = 0; i < counter; i++)
                {
                    if (PQgetisnull(result, 0, i) == 1)
                        list.append("");
                    else
                        list.append(PQgetvalue(result, 0, i));
                }

                // Занимаем её
                PQclear(result);
                result = manager->UpdateBid(0, "Обрабатывается администратором\v" + data_list[0] + '\v' + data_list[1]);
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
            if (message != "Направлена в таможню" && message != "Направлена на склад")
                message = "\x1A";
            else
                message = list.join('\v');
        }
    }

    return message;
}

QString AdministrationModule::UpdateBid(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Изменение заявки
    mutex->lock();
    PGresult* result = manager->UpdateBid(2, data_list.sliced(0, 29).join('\v') + '\v' +
                                                 QDate::currentDate().toString("yyyy-MM-dd") + '\v' + data_list[29]);
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

QString AdministrationModule::DeleteBid(QString data)
{
    // Результат работы
    QString message;
    int found;

    // Удаление заявки
    mutex->lock();

    PGresult* result = manager->SelectBid(1, data);

    if (result)
    {
        // Количество найденных заявок
        found = PQntuples(result);

        // Если заявка есть
        if (found > 0)
        {
            // Сохраняем её статус
            message = PQgetvalue(result, 0, 32);

            // Если заявка не занята
            if (message == "Направлена в таможню" || message == "Направлена на склад" || message == "Принята" || message == "Отказ")
            {
                // Удаляем её
                PQclear(result);
                result = manager->DeleteBid(data);
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
            if (message != "Направлена в таможню" && message != "Направлена на склад" && message != "Принята" && message != "Отказ")
                message = "\x1A";
            else
                message = "1";
        }
    }

    return message;
}

QString AdministrationModule::BidList(QString data)
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

QString AdministrationModule::CaseList(QString data)
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

QString AdministrationModule::ShipList(QString data)
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

QString AdministrationModule::SelectReadBid(QString data)
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

QString AdministrationModule::SelectReadCase(QString data)
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

QString AdministrationModule::SelectReadShip(QString data)
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

QString AdministrationModule::AddUser(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool username_unic = true;
    bool password_unic = true;

    // Добавление пользователя
    mutex->lock();

    PGresult* result = manager->SelectUser(1, data_list[4]);

    if (result)
    {
        // Проверка имени пользователя на уникальность
        username_unic = PQntuples(result) == 0;

        if (username_unic)
        {
            PQclear(result);

            result = manager->SelectUser(2, data_list[5]);

            if (result)
            {
                // Проверка пароля на уникальность
                password_unic = PQntuples(result) == 0;

                if (password_unic)
                {
                    PQclear(result);

                    result = manager->AddUser(data);
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
        if (!username_unic)
            message = "2";
        else if (!password_unic)
            message = "3";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString AdministrationModule::SelectUpdateUser(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool is_find = true;
    bool is_busy = false;

    // Поиск пользователя
    mutex->lock();

    PGresult* result1 = manager->SelectUser(0, data);
    PGresult* result2 = result1;

    if (result1)
    {
        is_find = PQntuples(result1) > 0;

        if (is_find)
        {
            result1 = manager->SelectUser(5, data_list[0]);

            if (result1)
                is_busy = QString(PQgetvalue(result1, 0, 0)).toInt() > 0;
        }
    }

    mutex->unlock();

    // Проверка результата
    if (!result1 || !result2)
    {
        message = "\x18";

        if (result1)
            PQclear(result1);

        if(result2)
            PQclear(result2);
    }
    else
    {
        if (!is_find)
        {
            message = "\x00";

            PQclear(result1);
        }
        else if (is_busy)
        {
            message = "\x1A";

            PQclear(result1);
            PQclear(result2);
        }
        else
        {
            // Упаковка данных
            QStringList list;
            int counter = PQnfields(result2);

            for (int i = 0; i < counter; i++)
            {
                if (PQgetisnull(result2, 0, i))
                    list.append("");
                else
                    list.append(PQgetvalue(result2, 0, i));
            }

            message = list.join('\v');

            PQclear(result1);
            PQclear(result2);
        }
    }

    return message;
}

QString AdministrationModule::UpdateUser(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool username_unic = true;
    bool password_unic = true;

    // Изменение пользователя
    mutex->lock();

    PGresult* result = manager->SelectUser(3, data_list[4] + '\v' + data_list[6] + '\v' + data_list[7]);

    if (result)
    {
        username_unic = PQntuples(result) == 0;

        if (username_unic)
        {
            PQclear(result);

            result = manager->SelectUser(4, data_list[5] + '\v' + data_list[6] + '\v' + data_list[7]);

            if (result)
            {
                password_unic = PQntuples(result) == 0;

                if (password_unic)
                {
                    PQclear(result);

                    result = manager->UpdateUser(data);
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
        if (!username_unic)
            message = "2";
        else if (!password_unic)
            message = "3";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString AdministrationModule::DeleteUser(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool is_find = true;
    bool is_busy = false;

    // Удаление пользователя
    mutex->lock();

    PGresult* result = manager->SelectUser(0, data);

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            PQclear(result);

            result = manager->SelectUser(5, data_list[0]);

            if (result)
            {
                is_busy = QString(PQgetvalue(result, 0, 0)).toInt() > 0;

                if (!is_busy)
                {
                    PQclear(result);

                    result = manager->DeleteUser(data);
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
        else
            message = "1";

        PQclear(result);
    }

    return message;
}
