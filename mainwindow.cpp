#include "mainwindow.h"

#include "apiclient.h"
#include "resumeanalyzerwidget.h"
#include "stylesheetloader.h"
#include "vacanciessearchwidget.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      page_stack(new QStackedWidget(this)),
      api_client(new ApiClient(this)),
      resume_button(nullptr)
{
    setWindowTitle(QStringLiteral("HH-Job-Assistant"));
    resize(1180, 780);
    setMinimumSize(860, 600);
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/mainwindow.qss")));

    QWidget *central = new QWidget(this);
    QHBoxLayout *root_layout = new QHBoxLayout(central);
    root_layout->setContentsMargins(0, 0, 0, 0);
    root_layout->setSpacing(0);

    QWidget *sidebar = new QWidget(central);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(224);
    QVBoxLayout *sidebar_layout = new QVBoxLayout(sidebar);
    sidebar_layout->setContentsMargins(16, 22, 16, 18);
    sidebar_layout->setSpacing(8);

    QHBoxLayout *brand_layout = new QHBoxLayout;
    brand_layout->setSpacing(11);
    QLabel *brand_mark = new QLabel(QStringLiteral("Н"), sidebar);
    brand_mark->setObjectName("brandMark");
    brand_mark->setAlignment(Qt::AlignCenter);
    QVBoxLayout *brand_text_layout = new QVBoxLayout;
    brand_text_layout->setSpacing(2);
    QLabel *brand_name = new QLabel(QStringLiteral("HH-Job-Assistant"), sidebar);
    brand_name->setObjectName("brandName");
    QLabel *brand_caption = new QLabel(QStringLiteral("карьерный помощник"), sidebar);
    brand_caption->setObjectName("brandCaption");
    brand_text_layout->addWidget(brand_name);
    brand_text_layout->addWidget(brand_caption);
    brand_layout->addWidget(brand_mark);
    brand_layout->addLayout(brand_text_layout, 1);
    sidebar_layout->addLayout(brand_layout);
    sidebar_layout->addSpacing(28);

    QLabel *menu_label = new QLabel(QStringLiteral("РАБОЧЕЕ ПРОСТРАНСТВО"), sidebar);
    menu_label->setObjectName("sidebarHint");
    menu_label->setContentsMargins(10, 0, 0, 6);
    sidebar_layout->addWidget(menu_label);

    QPushButton *search_button = new QPushButton(QStringLiteral("Поиск вакансий"), sidebar);
    search_button->setObjectName("navButton");
    search_button->setCheckable(true);
    search_button->setChecked(true);
    sidebar_layout->addWidget(search_button);

    resume_button = new QPushButton(QStringLiteral("Моё резюме"), sidebar);
    resume_button->setObjectName("navButton");
    resume_button->setCheckable(true);
    sidebar_layout->addWidget(resume_button);

    const QStringList coming_soon = {QStringLiteral("Сопроводительное письмо"),
                                     QStringLiteral("Избранное")};
    for (const QString &label : coming_soon) {
        QPushButton *button = new QPushButton(label, sidebar);
        button->setObjectName("navButton");
        button->setEnabled(false);
        sidebar_layout->addWidget(button);
    }
    sidebar_layout->addStretch();

    QButtonGroup *navigation_group = new QButtonGroup(this);
    navigation_group->setExclusive(true);
    navigation_group->addButton(search_button);
    navigation_group->addButton(resume_button);

    QWidget *vacancies_page = new VacanciesSearchWidget(api_client, page_stack);
    QWidget *resume_page = new ResumeAnalyzerWidget(api_client, page_stack);
    page_stack->addWidget(vacancies_page);
    page_stack->addWidget(resume_page);

    connect(search_button, &QPushButton::clicked, this, [this, vacancies_page]() {
        page_stack->setCurrentWidget(vacancies_page);
    });
    connect(resume_button, &QPushButton::clicked, this, [this, resume_page]() {
        page_stack->setCurrentWidget(resume_page);
    });

    root_layout->addWidget(sidebar);
    root_layout->addWidget(page_stack, 1);
    setCentralWidget(central);
}
