#include "vacancycard.h"

VacancyCard::VacancyCard(qint64 db_id,
                         qint64 vacancy_id,
                         const QString &name,
                         const QString &experience,
                         const QString &company_name,
                         const QString &area,
                         const QString &salary,
                         const QString &description,
                         QWidget *parent)
    : QFrame(parent),
      db_id(db_id),
      vacancy_id(vacancy_id),
      name_label(new QLabel(name)),
      experience_label(new QLabel(experience)),
      company_name_label(new QLabel(company_name)),
      area_label(new QLabel(area)),
      salary_label(new QLabel(salary)),
      description_label(new QLabel(description))
{
    setObjectName("vacancyCard");
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/vacancycard.qss")));

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(22, 19, 22, 19);
    main_layout->setSpacing(12);

    QHBoxLayout *title_layout = new QHBoxLayout;
    name_label->setObjectName("vacancyTitle");
    name_label->setWordWrap(true);
    title_layout->addWidget(name_label, 1);
    if (!salary.isEmpty()) {
        salary_label->setObjectName("vacancySalary");
        salary_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        title_layout->addWidget(salary_label, 0, Qt::AlignTop);
    } else {
        salary_label->hide();
    }
    main_layout->addLayout(title_layout);

    QHBoxLayout *meta_layout = new QHBoxLayout;
    meta_layout->setSpacing(8);
    company_name_label->setObjectName("vacancyMeta");
    area_label->setObjectName("vacancyMeta");
    experience_label->setObjectName("vacancyTag");
    meta_layout->addWidget(company_name_label);
    meta_layout->addWidget(new QLabel(QStringLiteral("·")));
    meta_layout->addWidget(area_label);
    meta_layout->addStretch();
    meta_layout->addWidget(experience_label);
    main_layout->addLayout(meta_layout);

    if (!description.isEmpty()) {
        description_label->setObjectName("vacancyDescription");
        description_label->setWordWrap(true);
        description_label->setMaximumHeight(42);
        main_layout->addWidget(description_label);
    } else {
        description_label->hide();
    }

    QPushButton *open_button = new QPushButton(QStringLiteral("Открыть на hh.ru"), this);
    open_button->setObjectName("vacancyOpenButton");
    open_button->setCursor(Qt::PointingHandCursor);
    QObject::connect(open_button, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://hh.ru/vacancy/%1").arg(this->vacancy_id)));
    });
    main_layout->addWidget(open_button, 0, Qt::AlignRight);
}

qint64 VacancyCard::dbId() const
{
    return db_id;
}

qint64 VacancyCard::vacancyId() const
{
    return vacancy_id;
}
