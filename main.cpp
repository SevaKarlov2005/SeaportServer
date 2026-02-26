#include <QCoreApplication>
#include <QTimer>

#include "distributionmodule.h"

int main(int argc, char *argv[])
{
    if (true) // argc == 2
    {
        //
        QCoreApplication a(argc, argv);

        //
        std::unique_ptr<DistributionModule> auth;

        //
        QTimer::singleShot(100,
                           [&auth]()
                           {
                               DatabaseManager* manager = new DatabaseManager("8080");
                               QMutex* mutex = new QMutex;

                               auth.reset(new DistributionModule(manager, mutex));


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
