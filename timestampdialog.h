#pragma once

#include "common.h"

namespace Ui {
class TimestampDialog;
}

class TimestampDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimestampDialog(QWidget *parent = nullptr);
    ~TimestampDialog();

private:
    Ui::TimestampDialog *ui;
    QStandardItemModel attendanceData;
    void UpdateAttendanceTable();
};
