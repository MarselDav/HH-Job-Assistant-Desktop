#ifndef STYLESHEETLOADER_H
#define STYLESHEETLOADER_H

#include <QFile>
#include <QString>

inline QString loadStyleSheet(const QString &resourcePath)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(file.readAll());
}

#endif // STYLESHEETLOADER_H
