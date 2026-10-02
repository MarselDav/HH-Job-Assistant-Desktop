#ifndef RESUMEANALYZERWIDGET_H
#define RESUMEANALYZERWIDGET_H

#include "apiclient.h"
#include "jsonformatter.h"
#include "resumecard.h"
#include "resumedropzone.h"
#include "resumehistorymanager.h"
#include "stylesheetloader.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QStringList>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

class ResumeAnalyzerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResumeAnalyzerWidget(ApiClient *apiClient,
                                  ResumeHistoryManager *resumeManager,
                                  QWidget *parent = nullptr);

    void setResumeAnalysis(const QJsonObject &analysis);

private slots:
    void uploadResume();
    void analyzeResume(const int &resumeID);
    void onResumeUploaded(const QJsonObject &resumeReply);
    void onResumeDelete();

private:
    void initializeResumes();
    void setAnalysisResumeCard(const QJsonObject &analysis);
    void selectResumeFile(const QString &path);
    void updateUploadButtonState();
    void addResumeCardsArray(const QJsonArray &resumesAnalysisArray);
    void addResumeCard(const int &resumeID,
                       const QString &title,
                       const QJsonObject &resumeAnalysis = QJsonObject());

    ApiClient *api_client;
    ResumeHistoryManager *resume_manager;
    QString selected_resume_path;
    QString pending_resume_title;
    QLabel *selected_file_label;
    QLabel *upload_status_label;
    QLabel *empty_resumes_label;
    QLineEdit *resume_name_field;
    QVBoxLayout *resume_cards_layout;
    QPushButton *upload_button;
    QPushButton *analyze_button;
    QTextBrowser *analysis_browser;

    int resume_id;
};

#endif // RESUMEANALYZERWIDGET_H
