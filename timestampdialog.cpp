#include "timestampdialog.h"
#include "ui_timestampdialog.h"
#include "database.h"

TimestampDialog::TimestampDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimestampDialog)
{
    ui->setupUi(this);
    // set default date
    ui->dateEdit->setDate(QDate::currentDate());
}

TimestampDialog::~TimestampDialog()
{
    delete ui;
}
