#include "searchfield.h"

SearchField::SearchField(QWidget *parent) : QLineEdit(parent)
{
    setObjectName("searchField");
    setPlaceholderText(QStringLiteral("Должность, ключевые слова или компания"));
    setClearButtonEnabled(true);
    setMinimumHeight(50);
    setStyleSheet(loadStyleSheet(QStringLiteral(":/stylesheets/searchfield.qss")));
}
