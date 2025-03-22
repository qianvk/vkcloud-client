#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include "singleton.h"
#include "global.h"

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

class HttpManager : public QObject, public Singleton<HttpManager>, public std::enable_shared_from_this<HttpManager>
{
    Q_OBJECT
    friend class Singleton<HttpManager>;
public:
    ~HttpManager() = default;

    void PostHttpRequest(QUrl url, const QJsonObject& json, RequestId req_id, Module mode);

public slots:
    void slot_http_finish(RequestId req_id, const QJsonObject& json_obj, Module mode);
private:
    HttpManager();

    QNetworkAccessManager network_manager_;

signals:
    void sig_http_finish(RequestId req_id, const QJsonObject& json_obj, Module mode);
    void sig_register_finish(RequestId req_id, const QJsonObject &json_obj);
    void SigLoginFinish(RequestId req_id, const QJsonObject &json_obj);
};

#endif // HTTPMANAGER_H
