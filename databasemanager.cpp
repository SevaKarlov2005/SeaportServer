#include "databasemanager.h"

#include <QDebug>
#include <QCoreApplication>

DatabaseManager::DatabaseManager(const char* port)
{
    // Подключаемся к базе данных
    this->connection = PQsetdbLogin("localhost", port, NULL, NULL, "seaport", "developer", "developer2026");

    // Проверяем удалось ли подключиться
    if (PQstatus(this->connection) != CONNECTION_OK)
    {
        // Выводим сообщение об ошибке в консоль
        qDebug() << PQerrorMessage(this->connection);

        // Очищаем память выделенную под соединение
        PQfinish(this->connection);
        this->connection = NULL;

        // Завершаем работу приложения
        QCoreApplication::quit();
    }
    else
    {
        // Сообщаем, что соединение с базой данных установлено
        qDebug() << "Database connection OK";

        // Указатель на результат выполнения запроса
        PGresult* result;

        // Запрещаем вывод предупреждений от базы данных в консоль
        result = PQexec(this->connection, "SET client_min_messages TO WARNING;");

        if (result == NULL)
        {
            qDebug() << "Error while setting up notifications";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error while setting up notifications";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы должностей
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS job_title "
                        "("
                        "job_id SERIAL PRIMARY KEY, "
                        "job_type VARCHAR(13) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы категории оборудования
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS equipment_category "
                        "("
                        "equipment_id SERIAL PRIMARY KEY, "
                        "equipment_type CHAR(1) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы статусов контейнера
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS container_status "
                        "("
                        "container_status_id SERIAL PRIMARY KEY, "
                        "container_status VARCHAR(30) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы размеров контейнера
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS external_dimensions "
                        "("
                        "dimensions_id SERIAL PRIMARY KEY, "
                        "size_code CHAR(2) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы типов контейнера
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS container_type "
                        "("
                        "container_type_id SERIAL PRIMARY KEY, "
                        "container_type CHAR(2) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы типов морского судна
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS ship_type "
                        "("
                        "ship_type_id SERIAL PRIMARY KEY, "
                        "ship_type CHAR(2) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы статусов морского судна
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS ship_status "
                        "("
                        "ship_status_id SERIAL PRIMARY KEY, "
                        "ship_status VARCHAR(30) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы статусов заявок
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS bid_status "
                        "("
                        "bid_status_id SERIAL PRIMARY KEY, "
                        "bid_status VARCHAR(30) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы складов
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS storage "
                        "("
                        "storage_id SERIAL PRIMARY KEY, "
                        "storage_type VARCHAR(10) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы транспорта
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS transport "
                        "("
                        "transport_id SERIAL PRIMARY KEY, "
                        "transport_type VARCHAR(10) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы направления
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS direction "
                        "("
                        "direction_id SERIAL PRIMARY KEY, "
                        "direction_type VARCHAR(10) NOT NULL UNIQUE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы сотрудника
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS employee "
                        "("
                        "employee_id SERIAL PRIMARY KEY, "
                        "surname VARCHAR(20) NOT NULL, "
                        "name VARCHAR(20) NOT NULL, "
                        "patronym VARCHAR(20) DEFAULT NULL, "
                        "job INT NOT NULL, "
                        "username VARCHAR(16) NOT NULL UNIQUE, "
                        "password VARCHAR(255) NOT NULL UNIQUE, "
                        "FOREIGN KEY (job) REFERENCES job_title (job_id) ON UPDATE RESTRICT ON DELETE CASCADE"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы заявок
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS bid "
                        "("
                        "bid_id SERIAL PRIMARY KEY, "
                        "bid_number CHAR(14) NOT NULL UNIQUE, "
                        "organization VARCHAR(40) NOT NULL, "
                        "INN CHAR(10) NOT NULL, "
                        "phone_number CHAR(12) NOT NULL, "
                        "email VARCHAR(40) NOT NULL, "
                        "cargo_name VARCHAR(50) NOT NULL, "
                        "TN_VED_code CHAR(10) NOT NULL, "
                        "direction INT NOT NULL, "
                        "length NUMERIC(5, 3) NOT NULL, "
                        "width NUMERIC(4, 3) NOT NULL, "
                        "height NUMERIC(4, 3) NOT NULL, "
                        "weight NUMERIC(8, 3) NOT NULL, "
                        "package TEXT NOT NULL, "
                        "transport_characteristics TEXT NOT NULL, "
                        "quantity INT NOT NULL, "
                        "transport_in INT NOT NULL, "
                        "transport_out INT NOT NULL, "
                        "date_in DATE NOT NULL, "
                        "date_out DATE NOT NULL, "
                        "storage INT NOT NULL, "
                        "shelf_life INT NOT NULL, "
                        "installation_requirements TEXT NOT NULL, "
                        "producer VARCHAR(40) NOT NULL, "
                        "shipper VARCHAR(40) NOT NULL, "
                        "port_out VARCHAR(40) NOT NULL, "
                        "country_out VARCHAR(40) NOT NULL, "
                        "port_in VARCHAR(40) NOT NULL, "
                        "country_in VARCHAR(40) NOT NULL, "
                        "consignee VARCHAR(40) NOT NULL, "
                        "other TEXT DEFAULT NULL, "
                        "comment TEXT DEFAULT NULL, "
                        "bid_status INT NOT NULL, "
                        "employee INT DEFAULT NULL, "
                        "date_add DATE NOT NULL, "
                        "date_change DATE NOT NULL, "
                        "FOREIGN KEY (direction) REFERENCES direction (direction_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (transport_in) REFERENCES transport (transport_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (transport_out) REFERENCES transport (transport_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (storage) REFERENCES storage (storage_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (bid_status) REFERENCES bid_status (bid_status_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (employee) REFERENCES employee (employee_id) ON UPDATE RESTRICT ON DELETE RESTRICT"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы морских судов
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS ship "
                        "("
                        "ship_id SERIAL PRIMARY KEY, "
                        "IMO_number CHAR(7) NOT NULL UNIQUE, "
                        "ship_type INT NOT NULL, "
                        "ship_name VARCHAR(30) NOT NULL, "
                        "ship_country VARCHAR(30) NOT NULL, "
                        "ship_status INT NOT NULL, "
                        "ship_payload NUMERIC(11, 3) NOT NULL, "
                        "ship_load NUMERIC(11, 3) NOT NULL, "
                        "employee INT DEFAULT NULL, "
                        "FOREIGN KEY (ship_type) REFERENCES ship_type (ship_type_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (ship_status) REFERENCES ship_status (ship_status_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (employee) REFERENCES employee (employee_id) ON UPDATE RESTRICT ON DELETE RESTRICT"
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Попытка создания таблицы контейнеров
        result = PQexec(this->connection,
                        "CREATE TABLE IF NOT EXISTS container "
                        "("
                        "container_id SERIAL PRIMARY KEY, "
                        "owner_code CHAR(3) NOT NULL, "
                        "equipment INT NOT NULL, "
                        "serial_number CHAR(6) NOT NULL UNIQUE, "
                        "check_number INT NOT NULL, "
                        "country_code CHAR(2) NOT NULL, "
                        "dimensions INT NOT NULL, "
                        "container_type INT NOT NULL, "
                        "gross_weight NUMERIC(8, 3) NOT NULL, "
                        "tare_weight NUMERIC(7, 3) NOT NULL, "
                        "container_payload NUMERIC(8, 3) NOT NULL, "
                        "capacity NUMERIC(6, 3) NOT NULL, "
                        "position CHAR(3) DEFAULT NULL, "
                        "container_status INT NOT NULL, "
                        "ship INT DEFAULT NULL, "
                        "bid INT DEFAULT NULL, "
                        "employee INT DEFAULT NULL, "
                        "FOREIGN KEY (equipment) REFERENCES equipment_category (equipment_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (dimensions) REFERENCES external_dimensions (dimensions_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (container_type) REFERENCES container_type (container_type_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (container_status) REFERENCES container_status (container_status_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (ship) REFERENCES ship (ship_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (bid) REFERENCES bid (bid_id) ON UPDATE RESTRICT ON DELETE RESTRICT, "
                        "FOREIGN KEY (employee) REFERENCES employee (employee_id) ON UPDATE RESTRICT ON DELETE RESTRICT "
                        ");");

        if (result == NULL)
        {
            qDebug() << "Error creating database tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            qDebug() << "Error creating database tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
            PQclear(result);

        // Сообщаем об успешном создании таблиц
        qDebug() << "Table creation was successful";

        // Заполнение таблицы статусов заявок
        result = PQexec(this->connection,
                        "SELECT * FROM bid_status;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO bid_status (bid_status) "
                                "VALUES "
                                "('Отказ'), "
                                "('Принята'), "
                                "('Обрабатывается на складе'), "
                                "('Направлена на склад'), "
                                "('Обрабатывается в таможне'), "
                                "('Направлена в таможню'), "
                                "('Обрабатывается администратором');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы статусов контейнеров
        result = PQexec(this->connection,
                        "SELECT * FROM container_status;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO container_status (container_status) "
                                "VALUES "
                                "('Свободен'), "
                                "('Обрабатывается на складе'), "
                                "('Занят'), "
                                "('Направлен диспетчеру'), "
                                "('Обрабатывается диспетчером'), "
                                "('Направлен стивидору'), "
                                "('Обрабатывается стивидором'), "
                                "('Погружен'), "
                                "('Не погружен');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы типов контейнера
        result = PQexec(this->connection,
                        "SELECT * FROM container_type;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO container_type (container_type) "
                                "VALUES "
                                "('G0'), "
                                "('G1'), "
                                "('G2'), "
                                "('G3'), "
                                "('V0'), "
                                "('V2'), "
                                "('V4'), "
                                "('R0'), "
                                "('R1'), "
                                "('R2'), "
                                "('R3'), "
                                "('UT'), "
                                "('TN'), "
                                "('HR'), "
                                "('B0'), "
                                "('B1'), "
                                "('B3'), "
                                "('B4'), "
                                "('B5'), "
                                "('B6'), "
                                "('PL'), "
                                "('S0'), "
                                "('S1'), "
                                "('S2');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы направлений
        result = PQexec(this->connection,
                        "SELECT * FROM direction;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO direction (direction_type) "
                                "VALUES "
                                "('Экспорт'), "
                                "('Импорт');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы категории оборудования
        result = PQexec(this->connection,
                        "SELECT * FROM equipment_category;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO equipment_category (equipment_type) "
                                "VALUES "
                                "('U'), "
                                "('Z'), "
                                "('R'), "
                                "('J');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы размеров контейнера
        result = PQexec(this->connection,
                        "SELECT * FROM external_dimensions;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO external_dimensions (size_code) "
                                "VALUES "
                                "('10'), "
                                "('18'), "
                                "('19'), "
                                "('20'), "
                                "('22'), "
                                "('28'), "
                                "('29'), "
                                "('30'), "
                                "('32'), "
                                "('35'), "
                                "('38'), "
                                "('39'), "
                                "('40'), "
                                "('42'), "
                                "('45'), "
                                "('48'), "
                                "('49'), "
                                "('L2'), "
                                "('L5');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы должностей
        result = PQexec(this->connection,
                        "SELECT * FROM job_title;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO job_title (job_type) "
                                "VALUES "
                                "('Администратор'), "
                                "('Таможенник'), "
                                "('Тальман'), "
                                "('Диспетчер'), "
                                "('Стивидор');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы статусов морского судна
        result = PQexec(this->connection,
                        "SELECT * FROM ship_status;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO ship_status (ship_status) "
                                "VALUES "
                                "('Свободно'), "
                                "('Обрабатывается'), "
                                "('Погрузка'), "
                                "('Уплыло');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы типов морского судна
        result = PQexec(this->connection,
                        "SELECT * FROM ship_type;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO ship_type (ship_type) "
                                "VALUES "
                                "('GP'), "
                                "('VH'), "
                                "('RE'), "
                                "('UT'), "
                                "('TN'), "
                                "('HR'), "
                                "('BU'), "
                                "('PL'), "
                                "('SN');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы типов склада
        result = PQexec(this->connection,
                        "SELECT * FROM storage;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO storage (storage_type) "
                                "VALUES "
                                "('Крытый'), "
                                "('Открытый');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Заполнение таблицы типов транспорта
        result = PQexec(this->connection,
                        "SELECT * FROM transport;");

        if (result == NULL)
        {
            qDebug() << "Error filling tables";
            QCoreApplication::quit();
        }
        else if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            qDebug() << "Error filling tables";
            PQclear(result);
            QCoreApplication::quit();
        }
        else
        {
            // Проверяем на пустоту
            if (PQntuples(result) == 0)
            {
                PQclear(result);
                result = PQexec(this->connection,
                                "INSERT INTO transport (transport_type) "
                                "VALUES "
                                "('Авто'), "
                                "('Ж/Д'), "
                                "('Водный');");

                if (result == NULL)
                {
                    qDebug() << "Error filling tables";
                    QCoreApplication::quit();
                }
                else if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    qDebug() << "Error filling tables";
                    PQclear(result);
                    QCoreApplication::quit();
                }
                else
                    PQclear(result);
            }
            else
                PQclear(result);
        }

        // Вывод сообщения об успешном заполнении таблиц в консоль
        qDebug() << "The tables have been filled in successfully";
    }
}

DatabaseManager::~DatabaseManager()
{
    // Если указатель на соединение не пуст, то очищаем его
    if (this->connection != NULL)
        PQfinish(this->connection);
}

PGresult* DatabaseManager::SelectBid(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Получение максимального идентификатора заявки;
        if (mod == 0)
            query = "SELECT MAX(bid_id) FROM bid;";

        // Получение заявки по идентификатору
        if (mod == 1)
        {
            query = "SELECT "
                    "b.bid_id, "
                    "b.bid_number, "
                    "b.organization, "
                    "b.inn, "
                    "b.phone_number, "
                    "b.email, "
                    "b.cargo_name, "
                    "b.tn_ved_code, "
                    "d.direction_type, "
                    "b.length, "
                    "b.width, "
                    "b.height, "
                    "b.weight, "
                    "b.package, "
                    "b.transport_characteristics, "
                    "b.quantity, "
                    "t1.transport_type, "
                    "t2.transport_type, "
                    "b.date_in, "
                    "b.date_out, "
                    "so.storage_type, "
                    "b.shelf_life, "
                    "b.installation_requirements, "
                    "b.producer, "
                    "b.shipper, "
                    "b.port_out, "
                    "b.country_out, "
                    "b.port_in, "
                    "b.country_in, "
                    "b.consignee, "
                    "b.other, "
                    "b.comment, "
                    "st.bid_status, "
                    "e.username, "
                    "b.date_add, "
                    "b.date_change "
                    "FROM bid AS b "
                    "JOIN direction AS d ON b.direction=d.direction_id "
                    "JOIN transport AS t1 ON b.transport_in=t1.transport_id "
                    "JOIN transport AS t2 ON b.transport_out=t2.transport_id "
                    "JOIN storage AS so ON b.storage=so.storage_id "
                    "JOIN bid_status AS st ON b.bid_status=st.bid_status_id "
                    "LEFT JOIN employee AS e ON b.employee=e.employee_id "
                    "WHERE b.bid_number='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение заявок по фильтрам и номеру страницы
        if (mod == 2)
        {
            query = "SELECT b.bid_number, b.date_add, b.date_change, s.bid_status FROM bid AS b "
                    "JOIN bid_status AS s ON b.bid_status=s.bid_status_id "
                    "WHERE %1 "
                    "ORDER BY %2 "
                    "LIMIT %3 OFFSET %4;";

            // Заготовка фильтра
            QString where = "";

            // Добавление идентификатора заявки
            if (data_list[4] != "")
            {
                if (where == "")
                    where += ("b.bid_number='" + data_list[4] + "'");
                else
                    where += (" AND b.bid_number='" + data_list[4] + "'");
            }

            // Добавление даты добавления
            if (data_list[5] != "")
            {
                if (where == "")
                    where += ("b.date_add='" + data_list[5] + "'");
                else
                    where += (" AND b.date_add='" + data_list[5] + "'");
            }

            // Добавление даты изменения
            if (data_list[6] != "")
            {
                if (where == "")
                    where += ("b.date_change='" + data_list[6] + "'");
                else
                    where += (" AND b.date_change='" + data_list[6] + "'");
            }

            // Добавление статуса заявки
            if (data_list[7] != "")
            {
                if (where == "")
                    where += ("s.bid_status='" + data_list[7] + "'");
                else
                    where += (" AND s.bid_status='" + data_list[7] + "'");
            }

            // Формирование запроса
            query = query.arg(where).arg(data_list[0] + ' ' + data_list[1]).arg(data_list[2]).arg(data_list[3]);
        }

        // Получение количества заявок по фильтрам
        if (mod == 3)
        {
            query = "SELECT COUNT(*) FROM bid AS b "
                    "JOIN bid_status AS s ON b.bid_status=s.bid_status_id "
                    "WHERE %1;";

            // Заготовка фильтра
            QString where = "";

            // Добавление идентификатора заявки
            if (data_list[0] != "")
            {
                if (where == "")
                    where += ("b.bid_number='" + data_list[0] + "'");
                else
                    where += (" AND b.bid_number='" + data_list[0] + "'");
            }

            // Добавление даты добавления
            if (data_list[1] != "")
            {
                if (where == "")
                    where += ("b.date_add='" + data_list[1] + "'");
                else
                    where += (" AND b.date_add='" + data_list[1] + "'");
            }

            // Добавление даты изменения
            if (data_list[2] != "")
            {
                if (where == "")
                    where += ("b.date_change='" + data_list[2] + "'");
                else
                    where += (" AND b.date_change='" + data_list[2] + "'");
            }

            // Добавление статуса заявки
            if (data_list[3] != "")
            {
                if (where == "")
                    where += ("s.bid_status='" + data_list[3] + "'");
                else
                    where += (" AND s.bid_status='" + data_list[3] + "'");
            }

            // Формирование запроса
            query = query.arg(where);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::AddBid(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        query = "INSERT INTO bid "
                "("
                "bid_number, "
                "organization, "
                "inn, "
                "phone_number, "
                "email, "
                "cargo_name, "
                "tn_ved_code, "
                "direction, "
                "length, "
                "width, "
                "height, "
                "weight, "
                "package, "
                "transport_characteristics, "
                "quantity, "
                "transport_in, "
                "transport_out, "
                "date_in, "
                "date_out, "
                "storage, "
                "shelf_life, "
                "installation_requirements, "
                "producer, "
                "shipper, "
                "port_out, "
                "country_out, "
                "port_in, "
                "country_in, "
                "consignee, "
                "other, "
                "bid_status, "
                "date_add, "
                "date_change"
                ") "
                "VALUES "
                "("
                "'%1', "
                "'%2', "
                "'%3', "
                "'%4', "
                "'%5', "
                "'%6', "
                "'%7', "
                "(SELECT direction_id FROM direction WHERE direction_type='%8'), "
                "%9, "
                "%10,"
                "%11, "
                "%12, "
                "'%13', "
                "'%14', "
                "%15, "
                "(SELECT transport_id FROM transport WHERE transport_type='%16'), "
                "(SELECT transport_id FROM transport WHERE transport_type='%17'), "
                "'%18', "
                "'%19', "
                "(SELECT storage_id FROM storage WHERE storage_type='%20'), "
                "%21, "
                "'%22', "
                "'%23', "
                "'%24', "
                "'%25', "
                "'%26', "
                "'%27', "
                "'%28', "
                "'%29', "
                "'%30', "
                "(SELECT bid_status_id FROM bid_status WHERE bid_status='%31'), "
                "'%32', "
                "'%33'"
                ");";

        // Формирование запроса
        query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]).arg(data_list[5])
                     .arg(data_list[6]).arg(data_list[7]).arg(data_list[8]).arg(data_list[9]).arg(data_list[10]).arg(data_list[11])
                     .arg(data_list[12]).arg(data_list[13]).arg(data_list[14]).arg(data_list[15]).arg(data_list[16]).arg(data_list[17])
                     .arg(data_list[18]).arg(data_list[19]).arg(data_list[20]).arg(data_list[21]).arg(data_list[22]).arg(data_list[23])
                     .arg(data_list[24]).arg(data_list[25]).arg(data_list[26]).arg(data_list[27]).arg(data_list[28]).arg(data_list[29])
                     .arg(data_list[30]).arg(data_list[31]).arg(data_list[32]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::UpdateBid(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Изменение статуса заявки по её идентификатору с привязкой сотрудника
        if (mod == 0)
        {
            query = "UPDATE bid "
                    "SET "
                    "bid_status=(SELECT bid_status_id FROM bid_status WHERE bid_status='%1'), "
                    "employee=(SELECT employee_id FROM employee WHERE username='%2') "
                    "WHERE bid_number='%3';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]);
        }

        // Изменение статуса заявки по её идентификатору с отвязкой сотрудника
        if (mod == 1)
        {
            query = "UPDATE bid "
                    "SET "
                    "bid_status=(SELECT bid_status_id FROM bid_status WHERE bid_status='%1'), "
                    "employee=NULL "
                    "WHERE bid_number='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Полное изменение заявки
        if (mod == 2)
        {
            query = "UPDATE bid "
                    "SET "
                    "organization='%1', "
                    "inn='%2', "
                    "phone_number='%3', "
                    "email='%4', "
                    "cargo_name='%5', "
                    "tn_ved_code='%6', "
                    "direction=(SELECT direction_id FROM direction WHERE direction_type='%7'), "
                    "length=%8, "
                    "width=%9, "
                    "height=%10, "
                    "weight=%11, "
                    "package='%12', "
                    "transport_characteristics='%13', "
                    "quantity=%14, "
                    "transport_in=(SELECT transport_id FROM transport WHERE transport_type='%15'), "
                    "transport_out=(SELECT transport_id FROM transport WHERE transport_type='%16'), "
                    "date_in='%17', "
                    "date_out='%18', "
                    "storage=(SELECT storage_id FROM storage WHERE storage_type='%19'), "
                    "shelf_life=%20, "
                    "installation_requirements='%21', "
                    "producer='%22', "
                    "shipper='%23', "
                    "port_out='%24', "
                    "country_out='%25', "
                    "port_in='%26', "
                    "country_in='%27', "
                    "consignee='%28', "
                    "other='%29', "
                    "date_change='%30' "
                    "WHERE bid_number='%31'";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]).arg(data_list[5])
                         .arg(data_list[6]).arg(data_list[7]).arg(data_list[8]).arg(data_list[9]).arg(data_list[10]).arg(data_list[11])
                         .arg(data_list[12]).arg(data_list[13]).arg(data_list[14]).arg(data_list[15]).arg(data_list[16]).arg(data_list[17])
                         .arg(data_list[18]).arg(data_list[19]).arg(data_list[20]).arg(data_list[21]).arg(data_list[22]).arg(data_list[23])
                         .arg(data_list[24]).arg(data_list[25]).arg(data_list[26]).arg(data_list[27]).arg(data_list[28]).arg(data_list[29])
                         .arg(data_list[30]);
        }

        // Изменение комментария по идентификатору заявки
        if (mod == 3)
        {
            query = "UPDATE bid "
                    "SET "
                    "comment='%1' "
                    "WHERE bid_number='%2'";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::DeleteBid(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Удаление заявки по идентификатору
        query = "DELETE FROM bid WHERE bid_number='%1'";
        query = query.arg(data_list[0]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::SelectUser(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Получение сотрудника по имени пользователя и паролю
        if (mod == 0)
        {
            query = "SELECT "
                    "e.employee_id, "
                    "e.surname, "
                    "e.name, "
                    "e.patronym, "
                    "j.job_type, "
                    "e.username, "
                    "e.password "
                    "FROM employee AS e "
                    "JOIN job_title AS j ON e.job=j.job_id "
                    "WHERE e.username='%1' AND e.password='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Получение сотрудника по имени пользователя
        if (mod == 1)
        {
            query = "SELECT "
                    "e.employee_id, "
                    "e.surname, "
                    "e.name, "
                    "e.patronym, "
                    "j.job_type, "
                    "e.username, "
                    "e.password "
                    "FROM employee AS e "
                    "JOIN job_title AS j ON e.job=j.job_id "
                    "WHERE e.username='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение сотрудника по паролю
        if (mod == 2)
        {
            query = "SELECT "
                    "e.employee_id, "
                    "e.surname, "
                    "e.name, "
                    "e.patronym, "
                    "j.job_type, "
                    "e.username, "
                    "e.password "
                    "FROM employee AS e "
                    "JOIN job_title AS j ON e.job=j.job_id "
                    "WHERE e.password='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение сотрудника по имени пользователя, исключая изменяемого
        if (mod == 3)
        {
            query = "SELECT "
                    "e.employee_id, "
                    "e.surname, "
                    "e.name, "
                    "e.patronym, "
                    "j.job_type, "
                    "e.username, "
                    "e.password "
                    "FROM employee AS e "
                    "JOIN job_title AS j ON e.job=j.job_id "
                    "WHERE e.username='%1' AND e.username!='%2' AND e.password!='%3';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]);
        }

        // Получение сотрудника по паролю, исключая изменяемого
        if (mod == 4)
        {
            query = "SELECT "
                    "e.employee_id, "
                    "e.surname, "
                    "e.name, "
                    "e.patronym, "
                    "j.job_type, "
                    "e.username, "
                    "e.password "
                    "FROM employee AS e "
                    "JOIN job_title AS j ON e.job=j.job_id "
                    "WHERE e.password='%1' AND e.username!='%2' AND e.password!='%3';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]);
        }

        // Занятость сотрудника
        if (mod == 5)
        {
            query = "SELECT COUNT(*) "
                    "FROM employee AS e "
                    "LEFT JOIN bid AS b ON e.employee_id=b.employee "
                    "LEFT JOIN container AS c ON e.employee_id=c.employee "
                    "LEFT JOIN ship AS s ON e.employee_id=s.employee "
                    "WHERE "
                    "e.username='%1' AND "
                    "(b.bid_id IS NOT NULL OR "
                    "c.container_id IS NOT NULL OR "
                    "s.ship_id IS NOT NULL)";
            query = query.arg(data_list[0]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::AddUser(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Добавление сотрудника со всеми полями
        query = "INSERT INTO employee "
                "("
                "surname, "
                "name, "
                "patronym, "
                "job, "
                "username, "
                "password"
                ") "
                "VALUES "
                "("
                "'%1', "
                "'%2', "
                "'%3', "
                "(SELECT job_id FROM job_title WHERE job_type='%4'), "
                "'%5', "
                "'%6'"
                ");";
        query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2])
                     .arg(data_list[3]).arg(data_list[4]).arg(data_list[5]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::UpdateUser(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Полное изменение сотрудника
        query = "UPDATE employee "
                "SET "
                "surname='%1', "
                "name='%2', "
                "patronym='%3', "
                "job=(SELECT job_id FROM job_title WHERE job_type='%4'), "
                "username='%5', "
                "password='%6'"
                "WHERE username='%7' AND password='%8';";
        query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2])
                     .arg(data_list[3]).arg(data_list[4]).arg(data_list[5])
                     .arg(data_list[6]).arg(data_list[7]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::DeleteUser(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Удаление сотрудника по имени пользователя и паролю
        query = "DELETE FROM employee WHERE username='%1' AND password='%2';";
        query = query.arg(data_list[0]).arg(data_list[1]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::SelectCase(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Получение контейнеров с определёнными типами
        if (mod == 0)
        {
            QString where = "'" + data_list.join("', '") + "'";
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE t.container_type IN (%1);";
            query = query.arg(where);
        }

        // Получение контейнеров по фильтрам и номеру страницы
        if (mod == 1)
        {
            query = "SELECT c.owner_code, e.equipment_type, c.serial_number, c.check_number, s.container_status "
                    "FROM container AS c "
                    "JOIN equipment_category AS e ON c.equipment=e.equipment_id "
                    "JOIN container_status AS s ON c.container_status=s.container_status_id "
                    "WHERE %1 "
                    "ORDER BY %2 "
                    "LIMIT %3 OFFSET %4;";

            QString where = "";

            // Добавление кода владельца
            if (data_list[4] != "")
            {
                if (where == "")
                    where += ("c.owner_code='" + data_list[4] + "'");
                else
                    where += (" AND c.owner_code='" + data_list[4] + "'");
            }

            // Добавление категории оборудования
            if (data_list[5] != "")
            {
                if (where == "")
                    where += ("e.equipment_type='" + data_list[5] + "'");
                else
                    where += (" AND e.equipment_type='" + data_list[5] + "'");
            }

            // Добавление серийного номера
            if (data_list[6] != "")
            {
                if (where == "")
                    where += ("c.serial_number='" + data_list[6] + "'");
                else
                    where += (" AND c.serial_number='" + data_list[6] + "'");
            }

            // Добавление контрольного числа
            if (data_list[7] != "")
            {
                if (where == "")
                    where += ("c.check_number=" + data_list[7]);
                else
                    where += (" AND c.check_number=" + data_list[7]);
            }

            // Добавление статуса контейнера
            if (data_list[8] != "")
            {
                if (where == "")
                    where += ("s.container_status='" + data_list[8] + "'");
                else
                    where += (" AND s.container_status='" + data_list[8] + "'");
            }

            query = query.arg(where).arg(data_list[0] + ' ' + data_list[1]).arg(data_list[2]).arg(data_list[3]);
        }

        // Получение количества контейнеров по фильтрам
        if (mod == 2)
        {
            query = "SELECT COUNT(*) "
                    "FROM container AS c "
                    "JOIN equipment_category AS e ON c.equipment=e.equipment_id "
                    "JOIN container_status AS s ON c.container_status=s.container_status_id "
                    "WHERE %1;";

            QString where = "";

            // Добавление кода владельца
            if (data_list[0] != "")
            {
                if (where == "")
                    where += ("c.owner_code='" + data_list[0] + "'");
                else
                    where += (" AND c.owner_code='" + data_list[0] + "'");
            }

            // Добавление категории оборудования
            if (data_list[1] != "")
            {
                if (where == "")
                    where += ("e.equipment_type='" + data_list[1] + "'");
                else
                    where += (" AND e.equipment_type='" + data_list[1] + "'");
            }

            // Добавление серийного номера
            if (data_list[2] != "")
            {
                if (where == "")
                    where += ("c.serial_number='" + data_list[2] + "'");
                else
                    where += (" AND c.serial_number='" + data_list[2] + "'");
            }

            // Добавление контрольного числа
            if (data_list[3] != "")
            {
                if (where == "")
                    where += ("c.check_number=" + data_list[3]);
                else
                    where += (" AND c.check_number=" + data_list[3]);
            }

            // Добавление статуса контейнера
            if (data_list[4] != "")
            {
                if (where == "")
                    where += ("s.container_status='" + data_list[4] + "'");
                else
                    where += (" AND s.container_status='" + data_list[4] + "'");
            }

            query = query.arg(where);
        }

        // Получение контейнера по его параметрам
        if (mod == 3)
        {
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE "
                    "c.owner_code='%1' AND eq.equipment_type='%2' AND c.serial_number='%3' AND c.check_number=%4 AND "
                    "st.container_status LIKE '%5';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]);
        }

        // Получение контейнеров с определёнными типами и статусом
        if (mod == 4)
        {
            QString where = "'" + data_list.sliced(1).join("', '") + "'";
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE t.container_type IN (%1) AND st.container_status='%2';";
            query = query.arg(where).arg(data_list[0]);
        }

        // Получение обрабатываемого конкретным тальманом контейнера
        if (mod == 5)
        {
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE "
                    "c.owner_code='%1' AND eq.equipment_type='%2' AND c.serial_number='%3' AND c.check_number=%4 "
                    "AND st.container_status='Обрабатывается на складе' AND e.username='%5'";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]);
        }

        // Получение количества контейнеров, принадлежащих тальману
        if (mod == 6)
        {
            query = "SELECT COUNT(*) "
                    "FROM container AS c "
                    "JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE e.username='%1'";
            query = query.arg(data_list[0]);
        }

        // Получение контейнеров, принадлежащих тальману и номеру страницы
        if (mod == 7)
        {
            query = "SELECT "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "st.container_status "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE "
                    "st.container_status='Обрабатывается на складе' AND e.username='%1' "
                    "ORDER BY %2 "
                    "LIMIT %3 OFFSET %4;";
            query = query.arg(data_list[0]).arg(data_list[1] + ' ' + data_list[2]).arg(data_list[3]).arg(data_list[4]);
        }

        // Получение контейнеров тальмана
        if (mod == 8)
        {
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE e.username='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение контейнера по его параметрам, исключая изменяемый
        if (mod == 9)
        {
            query = "SELECT "
                    "c.container_id, "
                    "c.owner_code, "
                    "eq.equipment_type, "
                    "c.serial_number, "
                    "c.check_number, "
                    "c.country_code, "
                    "ex.size_code, "
                    "t.container_type, "
                    "c.gross_weight, "
                    "c.tare_weight, "
                    "c.container_payload, "
                    "c.capacity, "
                    "c.position, "
                    "st.container_status, "
                    "s.IMO_number, "
                    "b.bid_number, "
                    "e.username "
                    "FROM container AS c "
                    "JOIN equipment_category AS eq ON c.equipment=eq.equipment_id "
                    "JOIN external_dimensions AS ex ON c.dimensions=ex.dimensions_id "
                    "JOIN container_type AS t ON c.container_type=t.container_type_id "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "LEFT JOIN ship AS s ON c.ship=s.ship_id "
                    "LEFT JOIN bid AS b ON c.bid=b.bid_id "
                    "LEFT JOIN employee AS e ON c.employee=e.employee_id "
                    "WHERE c.owner_code='%1' AND eq.equipment_type='%2' AND c.serial_number='%3' AND c.check_number=%4 "
                    "AND NOT (c.owner_code='%5' AND eq.equipment_type='%6' AND c.serial_number='%7' AND c.check_number=%8);";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3])
                         .arg(data_list[4]).arg(data_list[5]).arg(data_list[6]).arg(data_list[7]);
        }

        // Получение количества контейнеров по заявке
        if (mod == 10)
        {
            query = "SELECT COUNT(*) "
                    "FROM container AS c "
                    "JOIN bid AS b ON c.bid=b.bid_id "
                    "WHERE b.bid_number='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение количества контейнеров с определённым статусом по заявке
        if (mod == 11)
        {
            query = "SELECT COUNT(*) "
                    "FROM container AS c "
                    "JOIN container_status AS st ON c.container_status=st.container_status_id "
                    "JOIN bid AS b ON c.bid=b.bid_id "
                    "WHERE b.bid_number='%1' AND st.container_status='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::AddCase(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Добавление контейнера со всеми полями
        query = "INSERT INTO container "
                "("
                "owner_code, "
                "equipment, "
                "serial_number, "
                "check_number, "
                "country_code, "
                "dimensions, "
                "container_type, "
                "gross_weight, "
                "tare_weight, "
                "container_payload, "
                "capacity, "
                "container_status"
                ") "
                "VALUES "
                "("
                "'%1', "
                "(SELECT equipment_id FROM equipment_category WHERE equipment_type='%2'), "
                "'%3', "
                "%4, "
                "'%5', "
                "(SELECT dimensions_id FROM external_dimensions WHERE size_code='%6'), "
                "(SELECT container_type_id FROM container_type WHERE container_type='%7'), "
                "%8, "
                "%9, "
                "%10, "
                "%11, "
                "(SELECT container_status_id FROM container_status WHERE container_status='%12')"
                ");";
        query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3])
                     .arg(data_list[4]).arg(data_list[5]).arg(data_list[6]).arg(data_list[7])
                     .arg(data_list[8]).arg(data_list[9]).arg(data_list[10]).arg(data_list[11]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::UpdateCase(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Изменение статуса контейнера по его параметрам с привязкой сотрудника
        if (mod == 0)
        {
            query = "UPDATE container "
                    "SET "
                    "container_status=(SELECT container_status_id FROM container_status WHERE container_status='%1'), "
                    "employee=(SELECT employee_id FROM employee WHERE username='%2') "
                    "WHERE "
                    "owner_code='%3' AND "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%4') AND "
                    "serial_number='%5' AND "
                    "check_number=%6;";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]).arg(data_list[5]);
        }

        // Изменение статуса контейнера по его параметрам с отвязкой сотрудника
        if (mod == 1)
        {
            query = "UPDATE container "
                    "SET "
                    "container_status=(SELECT container_status_id FROM container_status WHERE container_status='%1'), "
                    "employee=NULL "
                    "WHERE "
                    "owner_code='%2' AND "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%3') AND "
                    "serial_number='%4' AND "
                    "check_number=%5;";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]);
        }

        // Изменение статусов контейнеров конкретного сотрудника
        if (mod == 2)
        {
            query = "UPDATE container "
                    "SET "
                    "container_status=(SELECT container_status_id FROM container_status WHERE container_status='%1') "
                    "WHERE "
                    "employee=(SELECT employee_id FROM employee WHERE username='%2');";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Изменение идентификатора заявки контейнеров конкретного сотрудника
        if (mod == 3)
        {
            query = "UPDATE container "
                    "SET "
                    "bid=(SELECT bid_id FROM bid WHERE bid_number='%1') "
                    "WHERE "
                    "employee=(SELECT employee_id FROM employee WHERE username='%2');";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Изменение морского судна и позиции у контейнера
        if (mod == 4)
        {
            query = "UPDATE container "
                    "SET "
                    "ship=(SELECT ship_id FROM ship WHERE IMO_number='%1'), "
                    "position='%2' "
                    "WHERE "
                    "owner_code='%3' AND "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%4') AND "
                    "serial_number='%5' AND "
                    "check_number=%6;";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]).arg(data_list[5]);
        }

        // Изменение статуса контейнеров и их отвязка
        if (mod == 5)
        {
            query = "UPDATE container "
                    "SET "
                    "container_status=(SELECT container_status_id FROM container_status WHERE container_status='%1'), "
                    "employee=NULL "
                    "WHERE employee=(SELECT employee_id FROM employee WHERE username='%2');";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Полное изменение контейнера
        if (mod == 6)
        {
            query = "UPDATE container "
                    "SET "
                    "owner_code='%1', "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%2'), "
                    "serial_number='%3', "
                    "check_number=%4, "
                    "country_code='%5', "
                    "dimensions=(SELECT dimensions_id FROM external_dimensions WHERE size_code='%6'), "
                    "container_type=(SELECT container_type_id FROM container_type WHERE container_type='%7'), "
                    "gross_weight=%8, "
                    "tare_weight=%9, "
                    "container_payload=%10, "
                    "capacity=%11 "
                    "WHERE "
                    "owner_code='%12' AND "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%13') AND "
                    "serial_number='%14' AND "
                    "check_number=%15;";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4])
                         .arg(data_list[5]).arg(data_list[6]).arg(data_list[7]).arg(data_list[8]).arg(data_list[9])
                         .arg(data_list[10]).arg(data_list[11]).arg(data_list[12]).arg(data_list[13]).arg(data_list[14]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::DeleteCase(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Удаление контейнера по его параметрам
        if (mod == 0)
        {
            query = "DELETE FROM container "
                    "WHERE "
                    "owner_code='%1' AND "
                    "equipment=(SELECT equipment_id FROM equipment_category WHERE equipment_type='%2') AND "
                    "serial_number='%3' AND "
                    "check_number=%4;";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]);
        }

        // Удаление контейнеров по заявке
        if (mod == 1)
        {
            query = "DELETE FROM container "
                    "WHERE bid=(SELECT bid_id FROM bid WHERE bid_number='%1');";
            query = query.arg(data_list[0]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::SelectShip(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Получение морских судов по фильтрам и номеру страницы
        if (mod == 0)
        {
            query = "SELECT s.IMO_number, s.ship_name, st.ship_status "
                    "FROM ship AS s "
                    "JOIN ship_status AS st ON s.ship_status=st.ship_status_id "
                    "WHERE %1 "
                    "ORDER BY %2 "
                    "LIMIT %3 OFFSET %4;";

            QString where = "";

            // Добавление IMO номера
            if (data_list[4] != "")
            {
                if (where == "")
                    where += ("s.IMO_number='" + data_list[4] + "'");
                else
                    where += (" AND s.IMO_number='" + data_list[4] + "'");
            }

            // Добавление названия судна
            if (data_list[5] != "")
            {
                if (where == "")
                    where += ("s.ship_name='" + data_list[5] + "'");
                else
                    where += (" AND s.ship_name='" + data_list[5] + "'");
            }

            // Добавление статуса судна
            if (data_list[6] != "")
            {
                if (where == "")
                    where += ("st.ship_status='" + data_list[6] + "'");
                else
                    where += (" AND st.ship_status='" + data_list[6] + "'");
            }

            query = query.arg(where).arg(data_list[0] + ' ' + data_list[1]).arg(data_list[2]).arg(data_list[3]);
        }

        // Получение количества морских судов по фильтрам
        if (mod == 1)
        {
            query = "SELECT COUNT(*) "
                    "FROM ship AS s "
                    "JOIN ship_status AS st ON s.ship_status=st.ship_status_id "
                    "WHERE %1;";

            QString where = "";

            // Добавление IMO номера
            if (data_list[0] != "")
            {
                if (where == "")
                    where += ("s.IMO_number='" + data_list[0] + "'");
                else
                    where += (" AND s.IMO_number='" + data_list[0] + "'");
            }

            // Добавление названия судна
            if (data_list[1] != "")
            {
                if (where == "")
                    where += ("s.ship_name='" + data_list[1] + "'");
                else
                    where += (" AND s.ship_name='" + data_list[1] + "'");
            }

            // Добавление статуса судна
            if (data_list[2] != "")
            {
                if (where == "")
                    where += ("st.ship_status='" + data_list[2] + "'");
                else
                    where += (" AND st.ship_status='" + data_list[2] + "'");
            }

            query = query.arg(where);
        }

        // Получение морского судна по IMO номеру
        if (mod == 2)
        {
            query = "SELECT "
                    "s.ship_id, "
                    "s.IMO_number, "
                    "t.ship_type, "
                    "s.ship_name, "
                    "s.ship_country, "
                    "st.ship_status, "
                    "s.ship_payload, "
                    "s.ship_load, "
                    "e.username "
                    "FROM ship AS s "
                    "JOIN ship_type AS t ON s.ship_type=t.ship_type_id "
                    "JOIN ship_status AS st ON s.ship_status=st.ship_status_id "
                    "LEFT JOIN employee AS e ON s.employee=e.employee_id "
                    "WHERE s.IMO_number='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение морских судов с определёнными типами, страной и статусом
        if (mod == 3)
        {
            QString where = "'" + data_list.sliced(2).join("', '") + "'";

            query = "SELECT "
                    "s.ship_id, "
                    "s.IMO_number, "
                    "t.ship_type, "
                    "s.ship_name, "
                    "s.ship_country, "
                    "st.ship_status, "
                    "s.ship_payload, "
                    "s.ship_load, "
                    "e.username "
                    "FROM ship AS s "
                    "JOIN ship_type AS t ON s.ship_type=t.ship_type_id "
                    "JOIN ship_status AS st ON s.ship_status=st.ship_status_id "
                    "LEFT JOIN employee AS e ON s.employee=e.employee_id "
                    "WHERE s.ship_country='%1' AND st.ship_status LIKE '%2' AND t.ship_type IN (%3);";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(where);
        }

        // Получение списка занятых мест на морском судне
        if (mod == 4)
        {
            query = "SELECT c.position "
                    "FROM ship AS s "
                    "JOIN container AS c ON s.ship_id=c.ship "
                    "WHERE s.IMO_number='%1';";
            query = query.arg(data_list[0]);
        }

        // Получение морского судна по IMO номеру, исключая изменяемое
        if (mod == 5)
        {
            query = "SELECT "
                    "s.ship_id, "
                    "s.IMO_number, "
                    "t.ship_type, "
                    "s.ship_name, "
                    "s.ship_country, "
                    "st.ship_status, "
                    "s.ship_payload, "
                    "s.ship_load, "
                    "e.username "
                    "FROM ship AS s "
                    "JOIN ship_type AS t ON s.ship_type=t.ship_type_id "
                    "JOIN ship_status AS st ON s.ship_status=st.ship_status_id "
                    "LEFT JOIN employee AS e ON s.employee=e.employee_id "
                    "WHERE s.IMO_number='%1' AND s.IMO_number!='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::AddShip(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Полное добавление морского судна
        query = "INSERT INTO ship"
                "("
                "IMO_number, "
                "ship_type, "
                "ship_name, "
                "ship_country, "
                "ship_status, "
                "ship_payload, "
                "ship_load"
                ") "
                "VALUES "
                "("
                "'%1', "
                "(SELECT ship_type_id FROM ship_type WHERE ship_type='%2'), "
                "'%3', "
                "'%4', "
                "(SELECT ship_status_id FROM ship_status WHERE ship_status='%5'), "
                "%6, "
                "%7"
                ");";
        query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3])
                     .arg(data_list[4]).arg(data_list[5]).arg(data_list[6]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::UpdateShip(unsigned short mod, QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Изменение статуса морского судна по его IMO номеру с привязкой сотрудника
        if (mod == 0)
        {
            query = "UPDATE ship "
                    "SET "
                    "ship_status=(SELECT ship_status_id FROM ship_status WHERE ship_status='%1'), "
                    "employee=(SELECT employee_id FROM employee WHERE username='%2') "
                    "WHERE IMO_number='%3';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]);
        }

        // Изменение статуса морского судна по его IMO номеру с отвязкой сотрудника
        if (mod == 1)
        {
            query = "UPDATE ship "
                    "SET "
                    "ship_status=(SELECT ship_status_id FROM ship_status WHERE ship_status='%1'), "
                    "employee=NULL "
                    "WHERE IMO_number='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Изменение загруженности морского судна по IMO номеру
        if (mod == 2)
        {
            query = "UPDATE ship "
                    "SET "
                    "ship_load=%1 "
                    "WHERE IMO_number='%2';";
            query = query.arg(data_list[0]).arg(data_list[1]);
        }

        // Полное изменение морского судна по IMO номеру
        if (mod == 3)
        {
            query = "UPDATE ship "
                    "SET "
                    "IMO_number='%1', "
                    "ship_type='%2', "
                    "ship_name='%3', "
                    "ship_country='%4', "
                    "ship_status='%5', "
                    "ship_payload=%6, "
                    "ship_load=%7 "
                    "WHERE IMO_number='%8';";
            query = query.arg(data_list[0]).arg(data_list[1]).arg(data_list[2]).arg(data_list[3]).arg(data_list[4]).arg(data_list[5])
                         .arg(data_list[6]).arg(data_list[7]);
        }

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}

PGresult* DatabaseManager::DeleteShip(QString data)
{
    // Подготовка запроса
    PGresult* result = NULL;
    QString query = "";

    // Извлечение параметров запроса
    QStringList data_list = data.split('\v');

    // Проверка на рабочее соединение
    if (PQstatus(this->connection) == CONNECTION_OK)
    {
        // Удаление морского судна по IMO номеру
        query = "DELETE FROM ship WHERE IMO_number='%1';";
        query = query.arg(data_list[0]);

        // Выполнение запроса
        result = PQexec(this->connection,
                        query.toUtf8().data());

        // Проверка результата на успешность
        if (result != NULL && PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            result = NULL;
        }
    }

    return result;
}
