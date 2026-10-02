#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "apiclient.h"
#include "resumeanalyzerwidget.h"
#include "resumehistorymanager.h"
#include "stylesheetloader.h"
#include "vacanciessearchwidget.h"

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QTranslator>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *page_stack;
    ApiClient *api_client;
    ResumeHistoryManager *resume_manager;
    QPushButton *resume_button;
};

#endif // MAINWINDOW_H
