#include <QCoreApplication>
#include <QTimer>

#include "customsmodule.h"

int main(int argc, char *argv[])
{
    if (true) // argc == 2
    {
        //
        QCoreApplication a(argc, argv);

        //
        std::unique_ptr<CustomsModule> auth;

        //
        QTimer::singleShot(100,
                           [&auth]()
                           {
                               DatabaseManager* manager = new DatabaseManager("8080");
                               QMutex* mutex = new QMutex;

                               auth.reset(new CustomsModule(manager, mutex));

                               QString data = "sidorov_s\v00004-24/ОМиДР";
                               QString result = auth->SelectWorkBid(data);

                               if (result == "\x00") {
                                   qDebug() << "❌ Заявка не найдена";
                               } else if (result == "\x1A") {
                                   qDebug() << "❌ Заявка не в статусе 'Направлена в таможню'";
                               } else if (result == "\x18") {
                                   qDebug() << "❌ Ошибка БД";
                               } else {
                                   qDebug() << "✅ Заявка взята в обработку, предыдущий статус:" << result;
                                   // Статус в БД теперь "Обрабатывается в таможне"
                               }
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
