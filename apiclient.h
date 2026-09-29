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

class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void getVacancies(const QJsonObject& object);
    void uploadResume(const QString& filePath, const QString& resumeName);
    void analyzeResume(const int& resumeID);

    // пока не работает
    // void getSemanticMatchingScores(const int& resume_id, const QList<int>& vacancies_ids);

signals:
    void vacanciesReceived(const QJsonArray &vacancies);
    void resumeUploaded(const QJsonObject &resumeReply);
    void resumeAnalyzed(const QJsonObject &resumeAnalysis);

private:
    QUrl endpoint(const QString &path);
    QUrl baseUrl;

    QNetworkAccessManager *manager;

signals:
};

#endif // APICLIENT_H
