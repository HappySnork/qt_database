#ifndef AUTH_H
#define AUTH_H

#include "mainwindow.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QWidget>


namespace Ui {
class auth;
}

class auth : public QWidget
{
    Q_OBJECT

public:
    explicit auth(QWidget *parent = nullptr);
    ~auth();

signals:
    void authorizationSuccessful(bool isAuthenticated);

private slots:
    void on_pushButton_connect_clicked();

private:
    Ui::auth *ui;
    MainWindow *mainWindow;

};

#endif // AUTH_H
