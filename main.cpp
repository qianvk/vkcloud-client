#include "mainwindow.h"

#include <QApplication>
#include <QFile>

#include "spdlog/spdlog.h"

#include "configurationmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/stylesheet.qss");

    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l] %s(%!):%# %v");
    try {
        if( qss.open(QFile::ReadOnly))
        {
            SPDLOG_INFO("open configuration file success");
            QString style = QLatin1String(qss.readAll());
            a.setStyleSheet(style);
            qss.close();
        }else{
            SPDLOG_CRITICAL("Open failed");
        }

        APPCONFIG.InitConfiguration();
    } catch (std::exception& e) {
        SPDLOG_CRITICAL(e.what());
    }
    MainWindow w;
    w.show();
    return a.exec();
}
