#ifndef RESUMECARD_H
#define RESUMECARD_H

#include <QFrame>

class QLabel;

class ResumeCard : public QFrame
{
    Q_OBJECT

public:
    explicit ResumeCard(const QString &resumeName, QWidget *parent = nullptr);

    QString resumeName() const;

private:
    QString resume_name;
    QLabel *resume_name_label;
};

#endif // RESUMECARD_H
