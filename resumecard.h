#ifndef RESUMECARD_H
#define RESUMECARD_H

#include "jsonformatter.h"
#include "stylesheetloader.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLayoutItem>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QScrollArea>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class ResumeCard : public QFrame
{
    Q_OBJECT

public:
    explicit ResumeCard(int resumeId,
                        const QString &resumeName,
                        const QJsonObject &resumeAnalysis = QJsonObject(),
                        QWidget *parent = nullptr);

    QString resumeName() const;
    void setResumeAnalysis(const QJsonObject &resumeAnalysis);

    int resume_id;

signals:
    void deleteResume(const int &resume_id);
    void resumeAnalyzeButtonClicked(const int &resume_id);

private:
    void addTopic(const QString &text);

    QString resume_name;
    QLabel *resume_name_label;
    QPushButton *analyze_button;
    QLabel *brief_description_label;
    QScrollArea *topics_scroll_area;
    QWidget *topics_container;
    QHBoxLayout *topics_layout;
    QToolButton *delete_button;
};

#endif // RESUMECARD_H
