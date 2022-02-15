#include "timestampdialog.h"
#include "ui_timestampdialog.h"
#include "database.h"

TimestampDialog::TimestampDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimestampDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->fromDateEdit, &QDateEdit::dateChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->toDateEdit, &QDateEdit::dateChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->fromDateCheckBox, &QCheckBox::stateChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->toDateCheckBox, &QCheckBox::stateChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->lineEditUserName, &QLineEdit::textChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->lineEditMatriculationNumber, &QLineEdit::textChanged, this, &TimestampDialog::UpdateAttendanceTable);
    connect(ui->pushButtonExport, &QPushButton::pressed, this, &TimestampDialog::ExportTable);
    // set default date
    ui->fromDateEdit->setDate(QDate::currentDate());
    ui->toDateEdit->setDate(QDate::currentDate());
    // set checkboxes to enable
    ui->fromDateCheckBox->setCheckState(Qt::CheckState::Checked);
    ui->toDateCheckBox->setCheckState(Qt::CheckState::Checked);
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

void TimestampDialog::ExportTable() {
    QString content = ConvertModelToCSV(attendanceData);
    SaveContentToFile(content, "anwesenheit_tabelle.csv");
}

void TimestampDialog::UpdateAttendanceTable() {
    // enable or disable date fields
    ui->fromDateEdit->setDisabled(!ui->fromDateCheckBox->isChecked());
    ui->toDateEdit->setDisabled(!ui->toDateCheckBox->isChecked());
    // retrieve attendance list
    QList<Attendance> attendanceList;
    try {
        QDate fromDate = ui->fromDateEdit->date();
        if (!ui->fromDateCheckBox->isChecked()) {
            fromDate = QDate();
        }
        QDate toDate = ui->toDateEdit->date();
        if (!ui->toDateCheckBox->isChecked()) {
            toDate = QDate();
        }
        attendanceList = GetAttendances(
                fromDate,
                toDate,
                ui->lineEditUserName->text(),
                ui->lineEditMatriculationNumber->text());
    } catch (QException e) {
        ShowMessage("Die Zeitstempel konnten nicht von der Datenbank geladen werden!");
    }
    attendanceData.clear();
    attendanceData.setHorizontalHeaderItem(0, new QStandardItem(QString("Datum")));
    attendanceData.setHorizontalHeaderItem(1, new QStandardItem(QString("Matrikelnummer")));
    attendanceData.setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));
    attendanceData.setHorizontalHeaderItem(3, new QStandardItem(QString("Anwesenheit")));
    for (Attendance& attendance : attendanceList) {
        std::map<QDate, QList<QTime>> timesPerDate;
        for (QDateTime& dateTime : attendance.timestampValues) {
            QDate date = dateTime.date();
            if (!timesPerDate.contains(date)) {
                timesPerDate[date] = {};
            }
            timesPerDate[date].append(dateTime.time());
        }
        for (auto& [date, times] : timesPerDate) {
            QList<QStandardItem*> itemList;
            itemList.append(new QStandardItem(date.toString(DATE_FORMAT)));
            itemList.append(new QStandardItem(attendance.matriculationNumber));
            itemList.append(new QStandardItem(attendance.userName));
            QStringList attendanceStringList;
            int index;
            for (index = 0; index < times.size() - 1; index += 2) {
                QTime arriveTime = times[index];
                QTime leaveTime = times[index + 1];
                qint64 attendanceMinutes = arriveTime.secsTo(leaveTime) / 60;
                attendanceStringList.append(arriveTime.toString(TIME_FORMAT) + "-" + leaveTime.toString(TIME_FORMAT) + " (" + QString::number(attendanceMinutes) + "min)");
            }
            if (index < times.size()) {
                attendanceStringList.append(times[index].toString(TIME_FORMAT));
            }
            itemList.append(new QStandardItem(attendanceStringList.join(", ")));
            attendanceData.appendRow(itemList);
        }
    }
    ui->attendanceTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}
