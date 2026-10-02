#include "vacanciessearchwidget.h"

VacanciesSearchWidget::VacanciesSearchWidget(ApiClient *apiClient, QWidget *parent)
    : QWidget(parent),
      api_client(apiClient),
      vacancies_scroll_area(new QScrollArea(this)),
      vacancies_layout(nullptr),
      search_field(new SearchField(this)),
      filters_button(new FilterButton(this))
{
    setObjectName("vacanciesSearchPage");
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/vacanciessearchwidget.qss")));

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(38, 32, 38, 28);
    main_layout->setSpacing(20);

    QLabel *eyebrow = new QLabel(QStringLiteral("ПОИСК РАБОТЫ"), this);
    eyebrow->setObjectName("pageEyebrow");
    main_layout->addWidget(eyebrow);

    QLabel *title = new QLabel(QStringLiteral("Найдите работу"), this);
    title->setObjectName("pageTitle");
    main_layout->addWidget(title);

    QLabel *subtitle = new QLabel(QStringLiteral("Подберём вакансии под ваши навыки и карьерные цели"), this);
    subtitle->setObjectName("pageSubtitle");
    main_layout->addWidget(subtitle);

    QHBoxLayout *search_layout = new QHBoxLayout;
    search_layout->setSpacing(12);
    search_layout->addWidget(search_field, 1);
    search_layout->addWidget(filters_button);
    main_layout->addLayout(search_layout);

    QHBoxLayout *results_header = new QHBoxLayout;
    QLabel *results = new QLabel(QStringLiteral("Рекомендуем вам"), this);
    results->setObjectName("resultsLabel");
    results_header->addWidget(results);
    results_header->addStretch();
    QLabel *count = new QLabel(QStringLiteral("Подборка для вас"), this);
    count->setObjectName("pageSubtitle");
    results_header->addWidget(count);
    main_layout->addLayout(results_header);

    QWidget *container = new QWidget;
    container->setObjectName("vacanciesList");
    vacancies_layout = new QVBoxLayout(container);
    vacancies_layout->setContentsMargins(1, 1, 8, 8);
    vacancies_layout->setSpacing(13);
    QLabel *empty_label = new QLabel(QStringLiteral("Вакансии появятся здесь после поиска"), container);
    empty_label->setObjectName("emptyVacanciesLabel");
    empty_label->setAlignment(Qt::AlignCenter);
    vacancies_layout->addWidget(empty_label);

    vacancies_scroll_area->setWidget(container);
    vacancies_scroll_area->setWidgetResizable(true);
    vacancies_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vacancies_scroll_area->setFrameShape(QFrame::NoFrame);
    main_layout->addWidget(vacancies_scroll_area, 1);

    connect(api_client, &ApiClient::vacanciesReceived,
            this, &VacanciesSearchWidget::setVacancies);
    connect(search_field, &QLineEdit::returnPressed,
            this, &VacanciesSearchWidget::requestVacancies);
}

void VacanciesSearchWidget::requestVacancies()
{
    QJsonObject request;
    request.insert(QStringLiteral("text"), search_field->text());
    api_client->getVacancies(request);
}

void VacanciesSearchWidget::setVacancies(const QJsonArray &vacancies)
{
    while (QLayoutItem *item = vacancies_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    if (vacancies.isEmpty()) {
        QLabel *empty_label = new QLabel(QStringLiteral("Подходящих вакансий не найдено"), vacancies_layout->parentWidget());
        empty_label->setObjectName("emptyVacanciesLabel");
        empty_label->setAlignment(Qt::AlignCenter);
        vacancies_layout->addWidget(empty_label);
        return;
    }

    for (const QJsonValue &value : vacancies) {
        if (!value.isObject())
            continue;

        const QJsonObject vacancy = value.toObject();
        const qint64 db_id = vacancy.value(QStringLiteral("db_id")).toVariant().toLongLong();
        const qint64 vacancy_id = vacancy.value(QStringLiteral("id")).toVariant().toLongLong();
        const QString name = vacancy.value(QStringLiteral("name")).toString();
        const QString work_schedule = vacancy.value(QStringLiteral("work_schedule")).toString();
        const QString company_name = vacancy.value(QStringLiteral("company_name")).toString();
        const QString area = vacancy.value(QStringLiteral("area")).toString();
        const QString experience = vacancy.value(QStringLiteral("experience")).toString();
        const QString salary = vacancy.value(QStringLiteral("salary")).toString();
        QString description = vacancy.value(QStringLiteral("recap")).toString();
        if (description.isEmpty())
            description = vacancy.value(QStringLiteral("description")).toString();

        QStringList meta_parts;
        if (!area.isEmpty())
            meta_parts.append(area);
        if (!work_schedule.isEmpty())
            meta_parts.append(work_schedule);

        vacancies_layout->addWidget(new VacancyCard(
            db_id, vacancy_id,
            name.isEmpty() ? QStringLiteral("Без названия") : name,
            experience,
            company_name.isEmpty() ? QStringLiteral("Компания не указана") : company_name,
            meta_parts.join(QStringLiteral(" · ")),
            salary, description, vacancies_layout->parentWidget()));
    }

    if (vacancies_layout->count() == 0) {
        QLabel *empty_label = new QLabel(QStringLiteral("Не удалось загрузить вакансии"), vacancies_layout->parentWidget());
        empty_label->setObjectName("emptyVacanciesLabel");
        empty_label->setAlignment(Qt::AlignCenter);
        vacancies_layout->addWidget(empty_label);
    } else {
        vacancies_layout->addStretch();
    }
}
