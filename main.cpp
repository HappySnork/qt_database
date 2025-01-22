#include "auth.h"
#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auth w;
    MainWindow ww;

    QObject::connect(&w, &auth::authorizationSuccessful, [&](bool isAuthenticated){
        ww.isAuthenticated = isAuthenticated;
        ww.show();
        w.close();
    });
    w.show();


    return a.exec();
}
