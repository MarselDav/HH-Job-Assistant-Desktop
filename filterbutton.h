#ifndef FILTERBUTTON_H
#define FILTERBUTTON_H

#include <QPushButton>
#include "stylesheetloader.h"

#include <QComboBox>
#include <QFormLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QPaintEvent>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOptionButton>
#include <QWidgetAction>
#include <QScrollArea>

class FilterButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FilterButton(QWidget *parent = nullptr);

    QString selectedCity() const;
    int minimumSalary() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QComboBox *city_combo;
    QSpinBox *salary_spin;
};

#endif // FILTERBUTTON_H
