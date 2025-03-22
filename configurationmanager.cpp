#include "configurationmanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

const QString &ConfigurationManager::GetVKCloudUrlPrefix() const
{
    return vkcloud_url_prefix_;
}

void ConfigurationManager::InitConfiguration()
{
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    // 拼接文件名
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path +
                                                   QDir::separator() + fileName);
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("VKCloudServer/host").toString();
    QString gate_port = settings.value("VKCloudServer/port").toString();
    vkcloud_url_prefix_ = "https://"+gate_host+":"+gate_port;
}

ConfigurationManager::ConfigurationManager() {}
