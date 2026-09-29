#ifndef RESUMEANALYZERWIDGET_H
#define RESUMEANALYZERWIDGET_H

#include <QJsonObject>
#include <QString>
#include <QWidget>

class ApiClient;
class QLabel;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QTextBrowser;

class ResumeAnalyzerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResumeAnalyzerWidget(ApiClient *apiClient, QWidget *parent = nullptr);
    void setResumeAnalysis(const QJsonObject &analysis);

private slots:
    void uploadResume();
    void analyzeResume();
    void onResumeUploaded(const QJsonObject &resumeReply);

private:
    void selectResumeFile(const QString &path);
    void updateUploadButtonState();

    ApiClient *api_client;
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
