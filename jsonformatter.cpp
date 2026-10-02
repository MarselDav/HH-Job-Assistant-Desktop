#include "jsonformatter.h"

QString JsonFormatter::jsonValueText(const QJsonValue &value)
{
    if (value.isString())
        return value.toString();
    if (value.isDouble())
        return QString::number(value.toDouble());
    if (value.isBool())
        return value.toBool() ? QStringLiteral("Да") : QStringLiteral("Нет");
    if (value.isObject())
        return QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
    return QString();
}

QString JsonFormatter::objectField(const QJsonObject &object, const QStringList &names)
{
    for (const QString &name : names) {
        const QString value = jsonValueText(object.value(name));
        if (!value.isEmpty())
            return value;
    }
    return QString();
}

QString JsonFormatter::formatItem(const QJsonValue &value, bool includeLevel)
{
    if (value.isString())
        return value.toString();
    if (!value.isObject())
        return jsonValueText(value);

    const QJsonObject object = value.toObject();
    QString name = objectField(object, {QStringLiteral("name"), QStringLiteral("skill_name"),
                                        QStringLiteral("language_name"), QStringLiteral("skill"),
                                        QStringLiteral("language"), QStringLiteral("title")});
    QString level;
    if (includeLevel) {
        level = objectField(object, {QStringLiteral("level"), QStringLiteral("proficiency_level"),
                                     QStringLiteral("proficiency"), QStringLiteral("level_name")});
    }

    if (name.isEmpty())
        name = jsonValueText(value);

    QString result = name;
    if (!level.isEmpty())
        result += QStringLiteral(" — ") + level;
    return result;
}

QStringList JsonFormatter::formatArray(const QJsonValue &value, bool includeLevel)
{
    QStringList formattedValues;
    if (!value.isArray())
        return formattedValues;

    QList<QJsonValue> items;
    for (const QJsonValue &item : value.toArray())
        items.append(item);

    const auto levelOf = [](const QJsonValue &item) {
        if (!item.isObject())
            return -std::numeric_limits<double>::infinity();

        const QString level = JsonFormatter::objectField(
            item.toObject(), {QStringLiteral("level"), QStringLiteral("proficiency_level"),
                              QStringLiteral("proficiency"), QStringLiteral("level_name")});
        bool ok = false;
        const double numericLevel = level.toDouble(&ok);
        return ok ? numericLevel : -std::numeric_limits<double>::infinity();
    };

    std::stable_sort(items.begin(), items.end(), [&levelOf](const QJsonValue &left, const QJsonValue &right) {
        return levelOf(left) > levelOf(right);
    });

    for (const QJsonValue &item : items) {
        const QString formatted = formatItem(item, includeLevel);
        if (!formatted.isEmpty())
            formattedValues.append(formatted);
    }

    return formattedValues;
}

QString JsonFormatter::experienceLabel(const QString &value)
{
    if (value == QLatin1String("noExperience")) return QStringLiteral("Нет опыта");
    if (value == QLatin1String("between1And3")) return QStringLiteral("От 1 до 3 лет");
    if (value == QLatin1String("between3And6")) return QStringLiteral("От 3 до 6 лет");
    if (value == QLatin1String("moreThan6")) return QStringLiteral("Более 6 лет");
    return value.isEmpty() ? QStringLiteral("Не указано") : value;
}
