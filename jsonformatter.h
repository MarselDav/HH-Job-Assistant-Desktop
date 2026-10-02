#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QStringList>
#include <algorithm>
#include <limits>

class JsonFormatter
{
public:
    static QStringList formatArray(const QJsonValue &value, bool includeLevel);
    static QString experienceLabel(const QString &value);

private:
    static QString jsonValueText(const QJsonValue &value);
    static QString objectField(const QJsonObject &object, const QStringList &names);
    static QString formatItem(const QJsonValue &value, bool includeLevel);
};

#endif // JSONFORMATTER_H
