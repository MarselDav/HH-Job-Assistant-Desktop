#include "resumehistorymanager.h"

ResumeHistoryManager::ResumeHistoryManager() {}



void ResumeHistoryManager::addResume(const int& resumeID)
{
    QSettings settings(RESUME_HISTORY_INI, QSettings::IniFormat);

    QVariantList list = settings.value(VALUE_NAME).toList();

    for (const QVariant& variant : list)
    {
        if (resumeID == variant.toInt())
        {
            qWarning() << QString("Резюме с таким id=%1 уже записано в историю!")
                              .arg(resumeID);
            return;
        }
    }

    list.append(resumeID);

    settings.setValue(VALUE_NAME, list);
}

void ResumeHistoryManager::deleteResume(const int& resumeID)
{
    QSettings settings(RESUME_HISTORY_INI, QSettings::IniFormat);

    QVariantList currentList = settings.value(VALUE_NAME).toList();
    QVariantList newList;

    for (const QVariant& value : currentList)
    {
        if (value.toInt() != resumeID)
            newList.append(value);
    }

    settings.value(VALUE_NAME, newList);
}

void ResumeHistoryManager::clearHistory()
{
    QSettings settings(RESUME_HISTORY_INI, QSettings::IniFormat);
    settings.setValue(VALUE_NAME, QVariantList());
}

QList<int> ResumeHistoryManager::getResumes()
{
    QSettings settings(RESUME_HISTORY_INI, QSettings::IniFormat);
    QVariantList list = settings.value(VALUE_NAME).toList();

    QList<int> intList;
    for (const QVariant& variant : list)
    {
        intList.append(variant.toInt());
    }

    return intList;
}
