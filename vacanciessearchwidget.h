#ifndef VACANCIESSEARCHWIDGET_H
#define VACANCIESSEARCHWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QFrame>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>
#include "apiclient.h"
#include "filterbutton.h"
#include "searchfield.h"
#include "stylesheetloader.h"
#include "vacancycard.h"

class VacanciesSearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VacanciesSearchWidget(ApiClient *apiClient, QWidget *parent = nullptr);
    void setVacancies(const QJsonArray &vacancies);

private slots:
    void requestVacancies();

private:
    ApiClient *api_client;
    QScrollArea *vacancies_scroll_area;
    QVBoxLayout *vacancies_layout;
    SearchField *search_field;
    FilterButton *filters_button;
};

#endif // VACANCIESSEARCHWIDGET_H
