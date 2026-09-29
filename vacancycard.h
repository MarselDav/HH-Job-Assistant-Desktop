#ifndef VACANCYCARD_H
#define VACANCYCARD_H

#include <QFrame>
#include <QtGlobal>

class QLabel;

class VacancyCard : public QFrame
{
    Q_OBJECT

public:
    explicit VacancyCard(qint64 db_id,
                         qint64 vacancy_id,
                         const QString &name,
                         const QString &experience,
                         const QString &company_name,
                         const QString &area,
                         const QString &salary = QString(),
                         const QString &description = QString(),
                         QWidget *parent = nullptr);

    qint64 dbId() const;
    qint64 vacancyId() const;

private:
    qint64 db_id;
    qint64 vacancy_id;
    QLabel *name_label;
    QLabel *experience_label;
    QLabel *company_name_label;
    QLabel *area_label;
    QLabel *salary_label;
    QLabel *description_label;
};

#endif // VACANCYCARD_H
