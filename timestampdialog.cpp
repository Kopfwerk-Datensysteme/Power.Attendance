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
    ui->attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
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
    auto attendanceList = GetAttendance(
                ui->dateEdit->date(),
                ui->lineEditUserName->text(),
                ui->lineEditMatriculationNumber->text());
    attendanceData.clear();
    attendanceData.setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    attendanceData.setHorizontalHeaderItem(1, new QStandardItem(QString("Matrikelnummer")));
    attendanceData.setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));
    attendanceData.setHorizontalHeaderItem(3, new QStandardItem(QString("Anwesenheit")));
    for (int row = 0; row < attendanceList.size(); row++) {
        QList<QStandardItem*> itemList;
        itemList.append(new QStandardItem(attendanceList[row].biometricId));
        itemList.append(new QStandardItem(attendanceList[row].matriculationNumber));
        itemList.append(new QStandardItem(attendanceList[row].userName));
        QStringList attendanceStringList;
        QList<qint64> timestampValues = attendanceList[row].timestampValues;
        int index;
        for (index = 0; index < timestampValues.size() - 1; index += 2) {
            attendanceStringList.append(QDateTime::fromSecsSinceEpoch(timestampValues[index]).time().toString() + "-" + QDateTime::fromSecsSinceEpoch(timestampValues[index + 1]).time().toString());
        }
        if (index < timestampValues.size()) {
            attendanceStringList.append(QDateTime::fromSecsSinceEpoch(timestampValues[index]).time().toString());
        }
        itemList.append(new QStandardItem(attendanceStringList.join(",")));
        attendanceData.appendRow(itemList);
    }
    ui->attendanceTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
