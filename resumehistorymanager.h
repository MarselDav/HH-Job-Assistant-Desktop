#ifndef RESUMEHISTORYMANAGER_H
#define RESUMEHISTORYMANAGER_H

#include <QSettings>
#include <QList>
#include <QString>

class ResumeHistoryManager
{
public:
    ResumeHistoryManager();

    void addResume(const int& resumeID);
    void deleteResume(const int& resumeID);
    void clearHistory();
    QList<int> getResumes();

private:
    const QString RESUME_HISTORY_INI = "ResumeHistory.ini";
    const QString VALUE_NAME = "Resumes/ids";
};

#endif // RESUMEHISTORYMANAGER_H
