#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdialog.h"
#include "timestampdialog.h"
#include "fingerprintreader.h"
#include "passwordchangedialog.h"
#include "database.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->actionModify, &QAction::triggered, this, &MainWindow::OnModify);
    connect(ui->actionCheck, &QAction::triggered, this, &MainWindow::OnCheck);
    connect(ui->actionPassword, &QAction::triggered, this, &MainWindow::OnChangeAdminPassword);
    connect(ui->pushButtonAddTimestamp, &QPushButton::pressed, this, &MainWindow::OnAddTimestamp);
    // setup fingerprint icon
    QIcon fingerprintImage(":/img/fingerprint.svg");
    ui->labelFingerprintImage->setPixmap(fingerprintImage.pixmap(200));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnModify() {
    if (!CheckAdminPassword()) {
        return;
    }
    UserDialog dlg;
    dlg.exec();
}

void MainWindow::OnCheck() {
    TimestampDialog dlg;
    dlg.exec();
}

void MainWindow::OnChangeAdminPassword() {
    if (!CheckAdminPassword()) {
        return;
    }
    PasswordChangeDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        try {
            UpdateSetting(SETTING_ADMIN_PASSWORD, dlg.newPassword);
        } catch (QException e) {
            ShowMessage("Der neue Admin-Passwort konnte nicht in der Datenbank gespeichert werden!");
            return;
        }
        ShowMessage("Das neue Admin-Passwort wurde erfolgreich in der Datenbank gespeichert!");
    }
}

void MainWindow::OnAddTimestamp() {
    QString biometricId;
    try {
        biometricId = GetBiometricIdForFingerprint();
        if (!DoesUserExist(biometricId)) {
            throw QException();
        }
    } catch (QException e) {
        ShowMessage("Der Benutzer konnte nicht identifiziert werden!");
        return;
    }
    try {
        AddTimestampForUser(biometricId);
    } catch (QException e) {
        ShowMessage("Der Zeitstempel konnte nicht in der Datenbank gespeichert werden!");
        return;
    }
    User currentUser = GetUser(biometricId);
    auto msgBox = GetNonModalMessageBox("Ein Zeitstempel für Benutzer \"" + currentUser.userName + "\" wurde erfolgreich der Datenbank hinzugefügt!");
    QThread::currentThread()->sleep(2);
    msgBox->close();
}
