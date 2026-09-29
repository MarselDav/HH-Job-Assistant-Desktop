#include "resumecard.h"

#include "stylesheetloader.h"

#include <QLabel>
#include <QVBoxLayout>

ResumeCard::ResumeCard(const QString &resumeName, QWidget *parent)
    : QFrame(parent),
      resume_name(resumeName),
      resume_name_label(new QLabel(resumeName, this))
{
    setObjectName("resumeCard");
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/resumecard.qss")));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->addWidget(resume_name_label);
    resume_name_label->setObjectName("resumeCardName");
    resume_name_label->setWordWrap(true);
}

QString ResumeCard::resumeName() const
{
    return resume_name;
}
