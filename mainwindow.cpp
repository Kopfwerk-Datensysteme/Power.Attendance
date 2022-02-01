#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdialog.h"
#include "timestampdialog.h"
#include "fingerprintreader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->actionModify, &QAction::triggered, this, &MainWindow::OnModify);
    connect(ui->actionCheck, &QAction::triggered, this, &MainWindow::OnCheck);
    // setup fingerprint icon
    QIcon fingerprintImage(":/img/fingerprint.svg");
    ui->labelFingerprintImage->setPixmap(fingerprintImage.pixmap(200));
    RegisterFingerprintForId("Hannes");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnModify() {
    UserDialog dlg;
    dlg.exec();
}

void MainWindow::OnCheck() {
    TimestampDialog dlg;
    dlg.exec();
}
