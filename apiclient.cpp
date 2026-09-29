#include "apiclient.h"

ApiClient::ApiClient(QObject *parent)
    : QObject{parent},
    manager(new QNetworkAccessManager(this)),
    baseUrl("http://localhost:8000")
{

}


QUrl ApiClient::endpoint(const QString &path)
{
    return baseUrl.resolved(QUrl(path));
}


void ApiClient::getVacancies(const QJsonObject& object)
{
    QNetworkRequest request;
    request.setUrl(endpoint("/get_vacancies/"));
    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
    );

    QJsonDocument document(object);

    QByteArray body = document.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = manager->post(request, body);

    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

        emit vacanciesReceived(document.array());
    });

}


void ApiClient::uploadResume(const QString& filePath, const QString& resumeName)
{
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly))
    {
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

    QUrl uploadResumeUrl = endpoint("/upload_resume/");

    QUrlQuery query;
    query.addQueryItem("name", resumeName);
    uploadResumeUrl.setQuery(query);

    QNetworkRequest request;
    request.setUrl(uploadResumeUrl);

    QNetworkReply* reply = manager->post(request, multipart);
    multipart->setParent(reply);


    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

        emit resumeUploaded(document.object());
    });
}


void ApiClient::analyzeResume(const int& resumeID)
{
    QUrl analyzeResumeUrl = endpoint("/analyze_resume/");

    analyzeResumeUrl.setPath(
        analyzeResumeUrl.path() + QString::number(resumeID)
    );

    QNetworkRequest request;
    request.setUrl(analyzeResumeUrl);

    QNetworkReply* reply = manager->get(request);


    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

        emit resumeAnalyzed(document.object());
    });
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
