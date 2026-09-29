#ifndef FILTERBUTTON_H
#define FILTERBUTTON_H

#include <QPushButton>

class QComboBox;
class QSpinBox;
class QPaintEvent;

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
