#include "distributionmodule.h"

#include <QMap>

DistributionModule::DistributionModule(DatabaseManager* manager, QMutex* mutex, QObject *parent) : QObject{parent}
{
    // Присвоение класса для взаимодействия с базой данных и мьютекса
    this->manager = manager;
    this->mutex = mutex;
}

void DistributionModule::ChooseAction(QString data)
{
    // Результат выполнения
    QString result = "";

    // Выбор действия
    if (data.sliced(0, 2) == "00")
        result = FreeBid(data.sliced(2));
    else if (data.sliced(0, 2) == "01")
        result = SelectWorkBid(data.sliced(2));
    else if (data.sliced(0, 2) == "02")
        result = SuggestDistribution(data.sliced(2));
    else if (data.sliced(0, 2) == "03")
        result = CheckDistribution(data.sliced(2));
    else if (data.sliced(0, 2) == "04")
        result = Refusal(data.sliced(2));
    else if (data.sliced(0, 2) == "05")
        result = Reception(data.sliced(2));
    else if (data.sliced(0, 2) == "06")
        result = FreeCases(data.sliced(2));
    else if (data.sliced(0, 2) == "07")
        result = NominatedCaseList(data.sliced(2));
    else if (data.sliced(0, 2) == "08")
        result = AssignCase(data.sliced(2));
    else if (data.sliced(0, 2) == "09")
        result = ReturnCase(data.sliced(2));
    else if (data.sliced(0, 2) == "10")
        result = BidList(data.sliced(2));
    else if (data.sliced(0, 2) == "11")
        result = CaseList(data.sliced(2));
    else if (data.sliced(0, 2) == "12")
        result = SelectReadBid(data.sliced(2));
    else if (data.sliced(0, 2) == "13")
        result = SelectReadCase(data.sliced(2));
    else if (data.sliced(0, 2) == "14")
        result = CheckCaseAddContents(data.sliced(2));
    else if (data.sliced(0, 2) == "15")
        result = AddCase(data.sliced(2));
    else if (data.sliced(0, 2) == "16")
        result = FreeCase(data.sliced(2));
    else if (data.sliced(0, 2) == "17")
        result = SelectUpdateCase(data.sliced(2));
    else if (data.sliced(0, 2) == "18")
        result = CheckCaseUpdateContents(data.sliced(2));
    else if (data.sliced(0, 2) == "19")
        result = UpdateCase(data.sliced(2));
    else if (data.sliced(0, 2) == "20")
        result = DeleteCaseByOptions(data.sliced(2));
    else if (data.sliced(0, 2) == "21")
        result = DeleteCaseByBid(data.sliced(2));

    // Отправка результата
    emit finish(result);
}

QString DistributionModule::FreeBid(QString data)
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

QString DistributionModule::SelectWorkBid(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QStringList list;
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
            if (message == "Направлена на склад")
            {
                // Извлекаем данные
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
                result = manager->UpdateBid(0, "Обрабатывается на складе\v" + data_list[0] + '\v' + data_list[1]);
            }
        }
    }

    mutex->unlock();

    // Проверка результата
    if (!result)
        message = "\x18";
    else
    {
        if (found == 0)
            message = "\x00";
        else
        {
            if (message != "Направлена на склад")
                message = "\x1A";
            else
                message = list.join('\v');
        }

        PQclear(result);
    }

    return message;
}

QString DistributionModule::SuggestDistribution(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');
    int cargo_length = qRound(data_list[0].toDouble() * 1000);
    int cargo_width = qRound(data_list[1].toDouble() * 1000);
    int cargo_height = qRound(data_list[2].toDouble() * 1000);
    int cargo_weight = qRound(data_list[3].toDouble() * 1000);
    int cargo_quantity = data_list[4].toInt();
    int code = data_list[5].sliced(0, 2).toInt();

    // Таблицы преобразований
    QMap<QChar, int> length_table;

    length_table.insert('1', 2991);
    length_table.insert('2', 6058);
    length_table.insert('3', 9125);
    length_table.insert('4', 2991);
    length_table.insert('L', 13716);

    QMap<QChar, int> height_table;

    height_table.insert('0', 2438);
    height_table.insert('2', 2591);
    height_table.insert('5', 2895);
    height_table.insert('8', 1295);
    height_table.insert('9', 1219);

    // Определение типа контейнеров
    QString case_group;

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

    // Результат работы
    QString message;
    bool is_find = true;
    int result_start = 0;
    int result_finish = -1;
    int case_count = 2147483647;

    // Назначение контейнеров
    mutex->lock();

    PGresult* result = manager->SelectCase(4, case_group + '\v' + "Свободен");

    if (result)
    {
        // Есть ли нужные контейнеры
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            QList<std::pair<int, QString>> case_list;
            int counter = PQntuples(result);
            int minimum;

            // Вычисление максимальной загрузки каждого контейнера
            for (int i = 0; i < counter; i++)
            {
                minimum = std::min({length_table[QString(PQgetvalue(result, i, 6))[0]] / cargo_length,
                                    2438 / cargo_width,
                                    height_table[QString(PQgetvalue(result, i, 6))[1]] / cargo_height,
                                    (int)qRound(QString(PQgetvalue(result, i, 10)).toDouble() * 1000) / cargo_weight});

                case_list.append(std::pair<int, QString>(minimum,
                                                         QString(PQgetvalue(result, i, 1)) + '\v' + QString(PQgetvalue(result, i, 2)) +
                                                             '\v' + QString(PQgetvalue(result, i, 3)) + '\v' + QString(PQgetvalue(result, i, 4))));
            }

            // Сортировка по возрастанию
            std::sort(case_list.begin(), case_list.end(), [](std::pair<int, QString> a, std::pair<int, QString> b) { return a.first < b.first; });

            // Поиск наименьшего числа контейнеров с максимальным использованием (метод двух указателей)
            int start = 0;
            int finish = -1;

            // Пока указатели в пределах списка
            while (start != counter && finish != counter)
            {
                // Если груз не упакован полностью
                if (cargo_quantity > 0)
                {
                    // Добавляем контейнер
                    finish++;

                    // Меняем количество оставшегося груза
                    if (finish != counter)
                        cargo_quantity -= case_list[finish].first;
                }
                else
                {
                    // Освобождаем контейнер
                    cargo_quantity += case_list[start].first;
                    start++;
                }

                // Если весь груз упакован
                if (start != counter && finish != counter && cargo_quantity <= 0)
                {
                    // Если потребовалось меньше контейнеров
                    if ((finish - start + 1) < case_count)
                    {
                        case_count = finish - start + 1;
                        result_start = start;
                        result_finish = finish;
                    }
                }
            }

            is_find = cargo_quantity <= 0;

            // Занимаем контейнеры
            if (is_find)
            {
                PQclear(result);

                for (int i = result_start; i <= result_finish; i++)
                {
                    result = manager->UpdateCase(0, "Обрабатывается на складе\v" + data_list[6] + '\v' + case_list[i].second);

                    if (result)
                        PQclear(result);
                    else
                        break;
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
        {
            message = "\x00";

            PQclear(result);
        }
        else
            message = "1";
    }

    return message;
}

QString DistributionModule::CheckDistribution(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');
    int cargo_length = qRound(data_list[0].toDouble() * 1000);
    int cargo_width = qRound(data_list[1].toDouble() * 1000);
    int cargo_height = qRound(data_list[2].toDouble() * 1000);
    int cargo_weight = qRound(data_list[3].toDouble() * 1000);
    int cargo_quantity = data_list[4].toInt();

    // Таблицы преобразований
    QMap<QChar, int> length_table;

    length_table.insert('1', 2991);
    length_table.insert('2', 6058);
    length_table.insert('3', 9125);
    length_table.insert('4', 2991);
    length_table.insert('L', 13716);

    QMap<QChar, int> height_table;

    height_table.insert('0', 2438);
    height_table.insert('2', 2591);
    height_table.insert('5', 2895);
    height_table.insert('8', 1295);
    height_table.insert('9', 1219);

    // Результат работы
    QString message;
    bool is_right = true;

    // Проверка распределения
    mutex->lock();

    // Получение контейнеров тальмана
    PGresult* result = manager->SelectCase(8, data_list[5]);

    if (result)
    {
        QList<int> case_list;
        int counter = PQntuples(result);
        int minimum;

        // Вычисление максимальной загрузки каждого контейнера
        for (int i = 0; i < counter; i++)
        {
            minimum = std::min({length_table[QString(PQgetvalue(result, i, 6))[0]] / cargo_length,
                                2438 / cargo_width,
                                height_table[QString(PQgetvalue(result, i, 6))[1]] / cargo_height,
                                (int)qRound(QString(PQgetvalue(result, i, 10)).toDouble() * 1000) / cargo_weight});

            case_list.append(minimum);
        }

        // Сортировка по убыванию
        std::sort(case_list.begin(), case_list.end(), [](int a, int b) { return a > b; });

        // Проверка на оптимальность
        for (int i = 0; i < counter; i++)
        {
            cargo_quantity -= case_list[i];

            // Если есть лишние контейнеры
            if (cargo_quantity <= 0 && i < (counter - 1))
            {
                is_right = false;
                break;
            }
        }

        // Нехватка контейнеров
        if (is_right)
            is_right = cargo_quantity <= 0;
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

QString DistributionModule::Refusal(QString data)
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

QString DistributionModule::Reception(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Снятие заявки и контейнеров с занятости
    mutex->lock();

    // Снятие заявки с занятости
    PGresult* result = manager->UpdateBid(1, data_list[0] + '\v' + data_list[1]);

    if (result)
    {
        PQclear(result);

        // Привязка заявки к контейнерам тальмана
        result = manager->UpdateCase(3, data_list[1] + '\v' + data_list[2]);

        if (result)
        {
            PQclear(result);

            // Снятие контейнеров с занятости
            result = manager->UpdateCase(5, "Направлен диспетчеру\v" + data_list[2]);
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

QString DistributionModule::FreeCases(QString data)
{
    // Результат работы
    QString message;

    // Снятие контейнеров с занятости
    mutex->lock();
    PGresult*result = manager->UpdateCase(5, "Свободен\v" + data);
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

QString DistributionModule::NominatedCaseList(QString data)
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
    PGresult* result = manager->SelectCase(6, data_list[4]);

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

        // Получение контейнеров тальмана для конкретной страницы
        result = manager->SelectCase(7, data_list[4] + '\v' + data_list.sliced(0, 2).join('\v') + '\v' + QString::number(limit) +
                                            '\v' + QString::number(minimum_tuples - 1));
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

QString DistributionModule::AssignCase(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool is_find = true;
    bool is_wrong = false;

    // Выбор группы контейнеров
    int code = data_list[6].sliced(0, 2).toInt();
    QStringList case_group;

    if ((code >= 1 && code <= 8) || code == 15 || (code >= 16 && code <= 21) || code == 30)
        case_group = {"R0", "R1", "R2", "R3"};
    else if (code == 22 || code == 27 || (code >= 28 && code <= 29) || (code >= 31 && code <= 38))
        case_group = {"UT", "TN", "HR"};
    else if ((code >= 9 && code <= 10) || code == 23 || (code >= 25 && code <= 26))
        case_group = {"B0", "B1", "B3", "B4", "B5", "B6"};
    else if (code >= 11 && code <= 14)
        case_group = {"V0", "V2", "V4"};
    else if ((code >= 44 && code <= 46) || (code >= 72 && code <= 83) || (code >= 86 && code <= 89))
        case_group = {"PL", "S0", "S1", "S2"};
    else
        case_group = {"G0", "G1", "G2", "G3"};

    // Назначение контейнера
    mutex->lock();

    // Получение контейнера для назначения
    PGresult* result = manager->SelectCase(3, data_list.sliced(0, 5).join('\v'));

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            is_wrong = !case_group.contains(PQgetvalue(result, 0, 7));

            if (!is_wrong)
            {
                PQclear(result);

                // Смена статуса у контейнера
                result = manager->UpdateCase(0, "Обрабатывается на складе\v" + data_list[5] + '\v' + data_list.sliced(0, 4).join('\v'));
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
        else if (is_wrong)
            message = "2";
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString DistributionModule::ReturnCase(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;
    bool is_find = true;

    // Снятие контейнера с назначения
    mutex->lock();

    PGresult* result = manager->SelectCase(5, data);

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            PQclear(result);

            result = manager->UpdateCase(1, "Свободен\v" + data_list.sliced(0, 4).join('\v'));
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
        else
            message = "1";

        PQclear(result);
    }

    return message;
}

QString DistributionModule::BidList(QString data)
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

QString DistributionModule::CaseList(QString data)
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

QString DistributionModule::SelectReadBid(QString data)
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
            (QString(PQgetvalue(result, 0, 32)) != "Направлена на склад" && QString(PQgetvalue(result, 0, 32)) != "Обрабатывается на складе"))
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

QString DistributionModule::SelectReadCase(QString data)
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

QString DistributionModule::CheckCaseAddContents(QString data)
{
    // Результат работы
    QString message;

    // Проверка существования контейнера
    mutex->lock();
    PGresult* result = manager->SelectCase(3, data + "\v%");
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

QString DistributionModule::AddCase(QString data)
{
    // Результат работы
    QString message;

    // Добавление контейнера
    mutex->lock();
    PGresult* result = manager->AddCase(data);
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

QString DistributionModule::FreeCase(QString data)
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

QString DistributionModule::SelectUpdateCase(QString data)
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

            if (message == "Свободен")
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
                result = manager->UpdateCase(0, "Обрабатывается на складе\v" + data);
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
            if (message != "Свободен")
                message = "\x1A";
            else
                message = list.join('\v');
        }
    }

    return message;
}

QString DistributionModule::CheckCaseUpdateContents(QString data)
{
    // Результат работы
    QString message;

    // Проверка существования контейнера
    mutex->lock();
    PGresult* result = manager->SelectCase(9, data);
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

QString DistributionModule::UpdateCase(QString data)
{
    // Извлечение данных
    QStringList data_list = data.split('\v');

    // Результат работы
    QString message;

    // Изменение контейнера
    mutex->lock();

    PGresult* result = manager->UpdateCase(6, data_list.sliced(1, 15).join('\v'));

    if (result)
    {
        PQclear(result);

        result = manager->UpdateCase(1, data_list[0] + '\v' + data_list.sliced(1, 4).join('\v'));
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

QString DistributionModule::DeleteCaseByOptions(QString data)
{
    // Результат работы
    QString message;
    bool is_find = true;
    bool is_busy = false;

    // Удаление контейнера по параметрам
    mutex->lock();

    PGresult* result = manager->SelectCase(3, data + "\v%");

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            is_busy = QString(PQgetvalue(result, 0, 13)) != "Свободен" && QString(PQgetvalue(result, 0, 13)) != "Погружен";

            if (!is_busy)
            {
                PQclear(result);

                result = manager->DeleteCase(0, data);
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

QString DistributionModule::DeleteCaseByBid(QString data)
{
    // Результат работы
    QString message;
    bool is_find = true;
    bool is_busy = false;

    // Удаление контейнеров по заявке
    mutex->lock();

    PGresult* result = manager->SelectBid(1, data);

    if (result)
    {
        is_find = PQntuples(result) > 0;

        if (is_find)
        {
            PQclear(result);

            result = manager->SelectCase(10, data);

            if (result)
            {
                int all = QString(PQgetvalue(result, 0, 0)).toInt();

                PQclear(result);

                result = manager->SelectCase(11, data + "\vПогружен");

                if (result)
                {
                    int success = QString(PQgetvalue(result, 0, 0)).toInt();
                    is_busy = all != success;

                    if (!is_busy)
                    {
                        PQclear(result);

                        result = manager->SelectCase(11, data + "\vНе погружен");

                        if (result)
                        {
                            int wrong = QString(PQgetvalue(result, 0, 0)).toInt();
                            is_busy = wrong == 0;

                            if (!is_busy)
                            {
                                PQclear(result);

                                result = manager->DeleteCase(1, data);
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
        else
            message = "1";

        PQclear(result);
    }

    return message;
}
