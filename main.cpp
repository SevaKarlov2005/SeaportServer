#include <QCoreApplication>
#include <QTimer>

#include "authorizationmodule.h"

int main(int argc, char *argv[])
{
    if (true) // argc == 2
    {
        //
        QCoreApplication a(argc, argv);

        //
        std::unique_ptr<AuthorizationModule> auth;

        //
        QTimer::singleShot(100,
                           [&auth]()
                           {
                               DatabaseManager* manager = new DatabaseManager("8080");
                               QMutex* mutex = new QMutex;

                               auth.reset(new AuthorizationModule(manager, mutex));

                               qDebug() << "";
                               auth->ChooseAction("00sidorov_s\vsidor123");
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
