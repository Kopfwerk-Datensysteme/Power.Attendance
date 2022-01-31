#include "timestampdialog.h"
#include "ui_timestampdialog.h"
#include "database.h"

TimestampDialog::TimestampDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimestampDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->lineEditUserName, &QLineEdit::textChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->lineEditMatriculationNumber, &QLineEdit::textChanged, this, &TimestampDialog::UpdateAttendanceTable);
    // set default date
    ui->dateEdit->setDate(QDate::currentDate());
    // set up user table
    ui->attendanceTable->setModel(&attendanceData);
    ui->attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->attendanceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->attendanceTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->attendanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    UpdateAttendanceTable();
}

TimestampDialog::~TimestampDialog()
{
    delete ui;
}

void TimestampDialog::UpdateAttendanceTable() {
    auto timestampList = GetAllTimestampsForDateUserNameMatriculationNumber(
                ui->dateEdit->date(),
                ui->lineEditUserName->text(),
                ui->lineEditMatriculationNumber->text());
    attendanceData.clear();
    attendanceData.setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    attendanceData.setHorizontalHeaderItem(1, new QStandardItem(QString("Matrikelnummer")));
    attendanceData.setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));
    attendanceData.setHorizontalHeaderItem(3, new QStandardItem(QString("Anwesenheit")));
    for (int row = 0; row < timestampList.size(); row++) {
        QList<QStandardItem*> itemList;
        itemList.append(new QStandardItem(timestampList[row].biometricId));
        itemList.append(new QStandardItem(timestampList[row].matriculationNumber));
        itemList.append(new QStandardItem(timestampList[row].userName));
        itemList.append(new QStandardItem(QString::number(timestampList[row].timestampValue)));
        attendanceData.appendRow(itemList);
    }
}
