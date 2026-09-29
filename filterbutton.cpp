#include "filterbutton.h"
#include "stylesheetloader.h"

#include <QComboBox>
#include <QFormLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionButton>
#include <QSpinBox>
#include <QWidgetAction>

FilterButton::FilterButton(QWidget *parent)
    : QPushButton(QStringLiteral("Фильтры"), parent),
      city_combo(new QComboBox),
      salary_spin(new QSpinBox)
{
    setObjectName("filtersButton");
    setMinimumHeight(50);
    setMinimumWidth(132);
    setCursor(Qt::PointingHandCursor);
    setIcon(QIcon(QStringLiteral(":/icons/images/filter.png")));
    setIconSize(QSize(18, 18));
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/filterbutton.qss")));

    QMenu *menu = new QMenu(this);
    menu->setObjectName("filtersMenu");
    menu->setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/filterbutton.qss")));

    QWidget *panel = new QWidget(menu);
    QFormLayout *form = new QFormLayout(panel);
    form->setContentsMargins(10, 8, 10, 10);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(12);

    city_combo->addItems({QStringLiteral("Любой город"), QStringLiteral("Москва"),
                          QStringLiteral("Санкт-Петербург"), QStringLiteral("Удалённо"),
                          QStringLiteral("Казань"), QStringLiteral("Екатеринбург")});
    salary_spin->setRange(0, 1000000);
    salary_spin->setSingleStep(10000);
    salary_spin->setSpecialValueText(QStringLiteral("Не указана"));
    salary_spin->setSuffix(QStringLiteral(" ₽"));
    salary_spin->setValue(0);

    form->addRow(QStringLiteral("Город"), city_combo);
    form->addRow(QStringLiteral("Зарплата от"), salary_spin);

    QWidgetAction *panel_action = new QWidgetAction(menu);
    panel_action->setDefaultWidget(panel);
    menu->addAction(panel_action);
    setMenu(menu);
}

QString FilterButton::selectedCity() const
{
    return city_combo->currentText();
}

int FilterButton::minimumSalary() const
{
    return salary_spin->value();
}

void FilterButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOptionButton option;
    initStyleOption(&option);
    option.text.clear();
    option.icon = QIcon();

    QPainter painter(this);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);

    const QRect content = rect().adjusted(16, 0, -16, 0);
    const int iconWidth = iconSize().width();
    const QRect iconRect(content.right() - iconWidth + 1,
                         content.center().y() - iconSize().height() / 2,
                         iconWidth, iconSize().height());
    const QRect textRect(content.left(), content.top(),
                         qMax(0, iconRect.left() - content.left() - 10), content.height());

    painter.setPen(option.palette.color(isEnabled() ? QPalette::Active : QPalette::Disabled,
                                        QPalette::ButtonText));
    painter.setFont(font());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    icon().paint(&painter, iconRect, Qt::AlignCenter,
                 isEnabled() ? QIcon::Normal : QIcon::Disabled);
}
