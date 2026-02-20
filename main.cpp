#include <QCoreApplication>
#include <QTimer>

#include "databasemanager.h"

int main(int argc, char *argv[])
{
    if (true) // argc == 2
    {
        //
        QCoreApplication a(argc, argv);

        //
        std::unique_ptr<DatabaseManager> manager;

        //
        QTimer::singleShot(100,
                           [&manager]()
                           {
                               manager = std::make_unique<DatabaseManager>("8080");

                               QString data = "\vТитаник\vСвободно";

                               PGresult* r = manager->SelectShip(1, data);
                               qDebug() << PQresultStatus(r);
                               int row = PQntuples(r);
                               int field = PQnfields(r);
                               qDebug() << row;
                               qDebug() << field;
                               for (int i = 0; i < row; i++)
                               {
                                   for (int j = 0; j < field; j++)
                                   {
                                       qDebug() << "row: " << i << ' ' << "field: " << j << "null: " << PQgetisnull(r, i, j) << "v: " << PQgetvalue(r, i, j);
                                   }
                               }

                               PQclear(r);
                           });

        //
        return a.exec();
    }
    else
    {
        //
        qDebug() << "Error in launch parameters";

        return 0;
    }
}
