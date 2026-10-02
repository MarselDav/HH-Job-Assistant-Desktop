#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>
#include <QUrlQuery>
#include <QByteArray>
#include <QDebug>
#include <QIODevice>
#include <QList>
#include <QUrl>

class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void getVacancies(const QJsonObject& object);
    void uploadResume(const QString& filePath, const QString& resumeName);
    void analyzeResume(const int& resumeID);

    void getResumes(const QList<int>& idsList);
    void deleteResume(const int& resumeID);

    // пока не работает
    // void getSemanticMatchingScores(const int& resume_id, const QList<int>& vacancies_ids);

private slots:
    void onReplyFinished(QNetworkReply* reply);

signals:
    void vacanciesReceived(const QJsonArray &vacancies);
    void resumeUploaded(const QJsonObject &resumeReply);
    void resumeAnalyzed(const QJsonObject &resumeAnalysis);
    void resumesGotten(const QJsonArray &resumesAnalysisArray);
    void resumeDeleted(bool status);

private:
    QUrl endpoint(const QString &path);
    QUrl baseUrl;

    QUrl getVacanciesUrl;
    QUrl uploadResumeUrl;
    QUrl analyzeResumeUrl;
    QUrl getResumesUrl;
    QUrl deleteResumeUrl;

    QNetworkAccessManager *manager;

signals:
};

/*
// 1. Создаем перечисление типов запросов в заголовочном файле
enum MyRequestType {
    AnalyzeResumeRequest = QNetworkRequest::User,
    LoginRequest,
    GetProfileRequest
};

// 2. При отправке запроса вешаем метку
QNetworkRequest request(url);
request.setAttribute(QNetworkRequest::User, AnalyzeResumeRequest); // Метка здесь!
manager->get(request);


int requestType = reply->request().attribute(QNetworkRequest::User).toInt();

*/

#endif // APICLIENT_H
