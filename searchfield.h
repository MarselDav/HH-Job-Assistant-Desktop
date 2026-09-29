#ifndef SEARCHFIELD_H
#define SEARCHFIELD_H

#include <QLineEdit>

class SearchField : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchField(QWidget *parent = nullptr);
};

#endif // SEARCHFIELD_H
