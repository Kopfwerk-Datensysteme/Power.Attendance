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
    QList<Attendance> attendanceList;
    try {
        attendanceList = GetAttendance(
                ui->dateEdit->date(),
                ui->lineEditUserName->text(),
                ui->lineEditMatriculationNumber->text());
    } catch (QException e) {
        ShowMessage("Die Zeitstempel konnten nicht von der Datenbank geladen werden!");
    }
    attendanceData.clear();
    attendanceData.setHorizontalHeaderItem(0, new QStandardItem(QString("Matrikelnummer")));
    attendanceData.setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    attendanceData.setHorizontalHeaderItem(2, new QStandardItem(QString("Anwesenheit")));
    for (int row = 0; row < attendanceList.size(); row++) {
        QList<QStandardItem*> itemList;
        itemList.append(new QStandardItem(attendanceList[row].matriculationNumber));
        itemList.append(new QStandardItem(attendanceList[row].userName));
        QStringList attendanceStringList;
        QList<QDateTime> timestampValues = attendanceList[row].timestampValues;
        int index;
        for (index = 0; index < timestampValues.size() - 1; index += 2) {
            QTime arriveTime = timestampValues[index].time();
            QTime leaveTime = timestampValues[index + 1].time();
            qint64 attendanceMinutes = arriveTime.secsTo(leaveTime) / 60;
            attendanceStringList.append(arriveTime.toString(TIME_FORMAT) + "-" + leaveTime.toString(TIME_FORMAT) + " (" + QString::number(attendanceMinutes) + "min)");
        }
        if (index < timestampValues.size()) {
            attendanceStringList.append(timestampValues[index].time().toString(TIME_FORMAT));
        }
        itemList.append(new QStandardItem(attendanceStringList.join(", ")));
        attendanceData.appendRow(itemList);
    }
    ui->attendanceTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
