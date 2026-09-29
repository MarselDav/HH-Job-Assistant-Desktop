#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QPushButton;
class ApiClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *page_stack;
    ApiClient *api_client;
    QPushButton *resume_button;
};

#endif // MAINWINDOW_H
