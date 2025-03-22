#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <QString>

#include "singleton.h"

class ConfigurationManager : public Singleton<ConfigurationManager>
{
    friend class Singleton<ConfigurationManager>;
public:
    const QString& GetVKCloudUrlPrefix() const;
    void InitConfiguration();
private:
    ConfigurationManager();

    QString vkcloud_url_prefix_;
};

#define APPCONFIG  (*ConfigurationManager::Instance())

#endif // CONFIGURATIONMANAGER_H
