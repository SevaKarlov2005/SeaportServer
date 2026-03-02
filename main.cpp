#include <QCoreApplication>
#include <QTimer>

#include "tcpserver.h"

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        bool ok;
        int port = QString(argv[1]).toInt(&ok);

        if (ok)
        {
            ok = (port > 0) && (port < 65536);

            if (ok)
            {
                QCoreApplication a(argc, argv);

                // Указатель на сервер
                std::unique_ptr<TCPServer> ptr;

                // Запуск сервера
                QTimer::singleShot(50, [&ptr, &argv](){ ptr.reset(new TCPServer(argv[1])); });

                return a.exec();
            }
            else
                qDebug() << "Invalid range";
        }
        else
            qDebug() << "Invalid parameter type";
    }
    else
        qDebug() << "Incorrect number of parameters";

    return 0;
}
