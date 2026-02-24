#include <QCoreApplication>
#include <QTimer>

#include "administrationmodule.h"

int main(int argc, char *argv[])
{
    if (true) // argc == 2
    {
        //
        QCoreApplication a(argc, argv);

        //
        std::unique_ptr<AdministrationModule> auth;

        //
        QTimer::singleShot(100,
                           [&auth]()
                           {
                               DatabaseManager* manager = new DatabaseManager("8080");
                               QMutex* mutex = new QMutex;

                               auth.reset(new AdministrationModule(manager, mutex));

                               QString data =
                                   "petrov_disp\v"       // 6 старый логин
                                   "petrov2026";        // 7 старый пароль

                               qDebug() << auth->DeleteUser(data);
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
