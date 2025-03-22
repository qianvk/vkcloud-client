#include "httpmanager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "spdlog/spdlog.h"

void HttpManager::PostHttpRequest(QUrl url, const QJsonObject& json, RequestId req_id, Module mode)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.size()));

    auto self = shared_from_this();
    QNetworkReply *reply = network_manager_.post(request, data);
    connect(reply, &QNetworkReply::finished, [self, reply, req_id, mode]() {
        QJsonObject json_obj;
        if (reply->error() != QNetworkReply::NoError) {
            SPDLOG_ERROR(reply->errorString().toStdString());
            json_obj["status"] = static_cast<int>(StatusCode::kErrNetwork);
            emit self->sig_http_finish(req_id, json_obj, mode);
            reply->deleteLater();
            return;
        }

        QString reply_data = reply->readAll();
        QJsonDocument json_doc = QJsonDocument::fromJson(reply_data.toUtf8());
        if (json_doc.isNull()) {
            SPDLOG_ERROR(EnumToString(StatusCode::kErrJson));
            json_obj["status"] = static_cast<int>(StatusCode::kErrJson);
            emit self->sig_http_finish(req_id, json_obj, mode);
            return;
        }

        json_obj = json_doc.object();
        emit self->sig_http_finish(req_id, json_obj, mode);
        reply->deleteLater();
        return;
    });

}

void HttpManager::slot_http_finish(RequestId req_id, const QJsonObject& json_obj, Module mode)
{
    switch (mode) {
    case Module::kRegister:
        emit sig_register_finish(req_id, json_obj);
        break;
    case Module::kLogin:
        emit SigLoginFinish(req_id, json_obj);
        break;
    default:
        break;
    }
}

HttpManager::HttpManager() {
    connect(this, &HttpManager::sig_http_finish, this, &HttpManager::slot_http_finish);
}
