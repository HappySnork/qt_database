#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "auth.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDate>
#include <QMessageBox>
#include <QTime>
#include <QTimeEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("smart_home_energy_monitoring");
    db.setUserName("postgres");
    db.setPassword("1234");

    if (!db.open()) {
        qDebug() << "Не удалось подключиться к базе данных!";
        return;
    }
    else
    {
        qDebug() << "kaif";
    }
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::onAuthorizationSuccessful(){
    qDebug() << "Всё заебись работаем";
}



void MainWindow::on_pushButton_refresh_clicked()
{
    if (isAuthenticated) {
        qDebug() << "Работаем";

        // Очищаем tableWidget
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);

        // Очищаем tableWidget_2
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);

        // Первая часть: Заполнение tableWidget
        QSqlQuery queryRooms;
        queryRooms.prepare("SELECT room_id, название FROM Комнаты");
        if (!queryRooms.exec()) {
            qDebug() << "Ошибка запроса к таблице Комнаты:" << queryRooms.lastError().text();
            return;
        }

        while (queryRooms.next()) {
            int roomId = queryRooms.value(0).toInt();
            QString roomName = queryRooms.value(1).toString();

            QSqlQuery queryDevices;
            queryDevices.prepare("SELECT название FROM Устройства WHERE room_id = :room_id");
            queryDevices.bindValue(":room_id", roomId);

            if (!queryDevices.exec()) {
                qDebug() << "Ошибка запроса к таблице Устройства:" << queryDevices.lastError().text();
                return;
            }

            while (queryDevices.next()) {
                QString deviceName = queryDevices.value(0).toString();

                int currentRow = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(currentRow);

                ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(roomName));
                ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(deviceName));

                ui->comboBox_device->addItem(deviceName);
            }
        }

        // Вторая часть: Заполнение tableWidget_2
        QSqlQuery queryCombined;
        QString combinedQuery = R"(
            SELECT Устройства.название AS "название", Пользователи.имя AS "имя",
                   Правила_устройств.время_начала AS "время_начала", Правила_устройств.время_окончания AS "время_окончания"
            FROM Устройства
            JOIN Правила_устройств ON Устройства.device_id = Правила_устройств.device_id
            JOIN Пользователи ON Правила_устройств.user_id = Пользователи.user_id
        )";

        if (!queryCombined.exec(combinedQuery)) {
            qDebug() << "Ошибка выполнения объединенного запроса:" << queryCombined.lastError().text();
            return;
        }

        while (queryCombined.next()) {
            QString deviceName = queryCombined.value(0).toString();
            QString userName = queryCombined.value(1).toString();
            QString startTime = queryCombined.value(2).toString();
            QString endTime = queryCombined.value(3).toString();

            int currentRow = ui->tableWidget_2->rowCount();
            ui->tableWidget_2->insertRow(currentRow);

            ui->tableWidget_2->setItem(currentRow, 0, new QTableWidgetItem(deviceName));
            ui->tableWidget_2->setItem(currentRow, 1, new QTableWidgetItem(userName));
            ui->tableWidget_2->setItem(currentRow, 2, new QTableWidgetItem(startTime));
            ui->tableWidget_2->setItem(currentRow, 3, new QTableWidgetItem(endTime));
        }

    }
}


void MainWindow::on_pushButton_AddSettings_clicked()
{
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Ошибка", "Вы не авторизованы!");
        return;
    }

    // Получаем выбранное устройство
    QString selectedDevice = ui->comboBox_device->currentText();
    if (selectedDevice.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите устройство!");
        return;
    }

    // Получаем настройку из lineEdit
    QString settingValue = ui->lineEdit->text();
    if (settingValue.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите настройку!");
        return;
    }

    // Получаем время начала и окончания
    QTime startTime = ui->timeEdit_start->time();
    QTime endTime = ui->timeEdit_end->time();

    // Проверяем, указано ли время
    if (!ui->timeEdit_start->time().isValid() || !ui->timeEdit_end->time().isValid()) {
        QMessageBox::warning(this, "Ошибка", "Время не указано!");
        return;
    }

    // Проверяем корректность времени (начало должно быть раньше окончания)
    if (startTime >= endTime) {
        QMessageBox::warning(this, "Ошибка", "Время начала должно быть раньше времени окончания!");
        return;
    }

    // Формируем строковое представление времени
    QDate currentDate = QDate::currentDate();
    QString startTimeStr = currentDate.toString("yyyy-MM-dd") + " " + startTime.toString("HH:mm:ss");
    QString endTimeStr = currentDate.toString("yyyy-MM-dd") + " " + endTime.toString("HH:mm:ss");

    // Получаем device_id из таблицы "Устройства" по названию устройства
    QSqlQuery queryDeviceId;
    queryDeviceId.prepare("SELECT device_id FROM Устройства WHERE название = :deviceName");
    queryDeviceId.bindValue(":deviceName", selectedDevice);

    if (!queryDeviceId.exec() || !queryDeviceId.next()) {
        QMessageBox::critical(this, "Ошибка", "Устройство не найдено!");
        qDebug() << "Ошибка запроса device_id:" << queryDeviceId.lastError().text();
        return;
    }

    int deviceId = queryDeviceId.value(0).toInt(); // Получаем device_id

    // Получаем user_id (добавьте логику получения текущего пользователя)
    int currentUserId = 1;
    int userId = currentUserId;

    // Вставляем данные в таблицу "Правила_устройств"
    QSqlQuery queryAddRule;
    QString insertRuleQuery = QString(
                                  "INSERT INTO Правила_устройств (device_id, user_id, время_начала, время_окончания) "
                                  "VALUES (%1, %2, '%3', '%4')")
                                  .arg(deviceId)
                                  .arg(userId)
                                  .arg(startTimeStr)
                                  .arg(endTimeStr);

    if (!queryAddRule.exec(insertRuleQuery)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить правило!");
        qDebug() << "Ошибка добавления правила:" << queryAddRule.lastError().text();
        return;
    }

    // Вставляем данные в таблицу "Настройки_устройств"
    QSqlQuery queryAddSetting;
    QString insertSettingQuery = QString(
                                     "INSERT INTO Настройки_устройств (device_id, режим, установленное_значение) "
                                     "VALUES (%1, '%2', %3)")
                                     .arg(deviceId)
                                     .arg(settingValue)
                                     .arg("NULL"); // NULL для установленного значения (если требуется значение, замените на нужное)

    if (!queryAddSetting.exec(insertSettingQuery)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить настройку!");
        qDebug() << "Ошибка добавления настройки:" << queryAddSetting.lastError().text();
        return;
    }

    QMessageBox::information(this, "Успех", "Настройка и правило успешно добавлены!");

}

