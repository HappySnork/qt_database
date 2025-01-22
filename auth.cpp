#include "auth.h"
#include "ui_auth.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDate>
#include <QMessageBox>
#include "mainwindow.h"

auth::auth(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::auth)
    , mainWindow(nullptr)
{
    ui->setupUi(this);
}

auth::~auth()
{
    delete ui;
}

void auth::on_pushButton_connect_clicked()
{
    QString enteredLogin = ui->lineEdit_login->text();
    QString enteredPassword = ui->lineEdit_pass->text();

    // Создаем запрос для проверки логина и пароля
    QSqlQuery query;
    query.prepare("SELECT имя, пароль FROM Пользователи WHERE имя = :name");
    query.bindValue(":name", enteredLogin);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса для проверки логина:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Ошибка выполнения запроса для проверки логина.");
        return;
    }

    // Проверяем, если пользователь найден, то сверяем пароль
    if (query.next()) {
        QString storedPassword = query.value("пароль").toString();

        // Сравниваем введённый пароль с паролем из базы данных
        if (enteredPassword == storedPassword) {
            qDebug() << "Авторизация успешна!";
            QMessageBox::information(this, "Успех", "Авторизация прошла успешно!");
            emit authorizationSuccessful(true);
        } else {
            qDebug() << "Неверный пароль!";
            QMessageBox::warning(this, "Ошибка", "Неверный пароль!");
            emit authorizationSuccessful(false);
        }
    } else {
        qDebug() << "Пользователь не найден!";
        QMessageBox::warning(this, "Ошибка", "Пользователь не найден!");
    }
}

