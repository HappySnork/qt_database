#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    bool isAuthenticated;

public slots:
    void onAuthorizationSuccessful();


private slots:


    void on_pushButton_refresh_clicked();

    void on_pushButton_AddSettings_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase*db = new QSqlDatabase;
    bool authSuccessfuf;


};
#endif // MAINWINDOW_H
