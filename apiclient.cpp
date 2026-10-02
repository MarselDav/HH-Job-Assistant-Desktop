#include "apiclient.h"

ApiClient::ApiClient(QObject *parent)
    : QObject{parent},
    manager(new QNetworkAccessManager(this)),
    baseUrl("http://localhost:8000")
{
    getVacanciesUrl = endpoint("/get_vacancies/");
    uploadResumeUrl = endpoint("/upload_resume/");
    analyzeResumeUrl  = endpoint("/analyze_resume/");
    getResumesUrl  = endpoint("/get_resumes/");
    deleteResumeUrl  = endpoint("/delete_resume/");

    connect(manager, &QNetworkAccessManager::finished, this, &ApiClient::onReplyFinished);
}

QUrl ApiClient::endpoint(const QString &path)
{
    return baseUrl.resolved(QUrl(path));
}

void ApiClient::onReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "[ApiClient][onReplyFinished] Сетевая ошибка!";

        if (reply->error() == QNetworkReply::TimeoutError) {
            qCritical() << "[ApiClient][onReplyFinished] Превышено время ожидания от сервера!";
        }
        else if (reply->error() == QNetworkReply::HostNotFoundError) {
            qCritical() << "[ApiClient][onReplyFinished] Проблема с DNS иил адресом сервера!";
        }

        return;
    }

    QUrl url = reply->url();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode >= 400){
        qWarning() << "HTTP ошибка сервера. Код статуса:" << statusCode;

        if (url.adjusted(QUrl::RemoveQuery) == deleteResumeUrl) {
            emit resumeDeleted(false);
        }

        QByteArray errorBody = reply->readAll();
        if (!errorBody.isEmpty()) {
            qWarning() << "Тело ошибки от сервера:" << errorBody;
        }

        return;
    }


    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (url == getVacanciesUrl) {
        emit vacanciesReceived(document.array());
    }
    else if (url.adjusted(QUrl::RemoveQuery) == uploadResumeUrl) {
        emit resumeUploaded(document.object());
    }
    else if (url.adjusted(QUrl::RemoveFilename) == analyzeResumeUrl) {
        emit resumeAnalyzed(document.object());
    }
    else if (url == getResumesUrl) {
        emit resumesGotten(document.array());
    }
    else if (url.adjusted(QUrl::RemoveQuery) == deleteResumeUrl) {
        emit resumeDeleted(true);
    }
    else {
        qWarning() << "[ApiClient][onReplyFinished] Неизвестный path запроса!";
    }
}


void ApiClient::getVacancies(const QJsonObject& object) {
    QNetworkRequest request;
    request.setUrl(getVacanciesUrl);
    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
    );

    QJsonDocument document(object);

    QByteArray body = document.toJson(QJsonDocument::Compact);
    manager->post(request, body);
}


void ApiClient::uploadResume(const QString& filePath, const QString& resumeName) {
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Не удалось открыть файл с резюме";
    }

    QHttpMultiPart *multipart = new QHttpMultiPart(
        QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"file\"; filename=\"%1\"").
        arg(QFileInfo(*file).fileName()));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    filePart.setBodyDevice(file);

    multipart->setParent(file);
    multipart->append(filePart);

    QUrlQuery query;

    QByteArray encodedName = QUrl::toPercentEncoding(resumeName);
    query.addQueryItem("name", QString::fromUtf8(encodedName));

    QUrl uRUrl = uploadResumeUrl;
    uRUrl.setQuery(query);

    QNetworkRequest request;
    request.setUrl(uRUrl);

    manager->post(request, multipart);
}


void ApiClient::analyzeResume(const int& resumeID)
{
    QUrl aRUrl = analyzeResumeUrl;
    aRUrl.setPath(
        aRUrl.path() + QString::number(resumeID)
    );

    QNetworkRequest request;
    request.setUrl(aRUrl);

    manager->get(request);
}

void ApiClient::getResumes(const QList<int>& idsList)
{
    QNetworkRequest request;
    request.setUrl(getResumesUrl);
    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
    );

    QJsonArray jsonArray;
    for (int id : idsList)
    {
        jsonArray.append(id);
    }

    QJsonDocument document(jsonArray);

    QByteArray body = document.toJson(QJsonDocument::Compact);

    manager->post(request, body);
}


void ApiClient::deleteResume(const int& resumeID)
{
    QUrlQuery query;
    query.addQueryItem("resume_id", QString::number(resumeID));
    QUrl dRUrl = deleteResumeUrl;
    dRUrl.setQuery(query);

    QNetworkRequest request;
    request.setUrl(dRUrl);

    manager->deleteResource(request);
}

// void ApiClient::getSemanticMatchingScores(const int& resume_id, const QList<int>& vacancies_ids)
// {
//     QNetworkRequest request;
//     request.setUrl(endpoint("/semantic_matching/"));
//     request.setHeader(
//         QNetworkRequest::ContentTypeHeader,
//         "application/json"
//         );

//     QJsonObject object;
//     // object.insert(QStringLiteral("resume_id"), resume_id);
//     // object.insert(QStringLiteral("vacancies_ids"), vacancies_ids);

//     // QJsonObject
//     // request.insert(QStringLiteral("text"), search_field->text());

//     QJsonDocument document(object);

//     QByteArray body = document.toJson(QJsonDocument::Compact);

//     QNetworkReply *reply = manager->post(request, body);

//     connect(reply, &QNetworkReply::finished, this, [this, reply]()
//             {
//                 QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

//                 emit vacanciesReceived(document.array());
//             });
// }
