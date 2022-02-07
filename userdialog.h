#pragma once

#include "common.h"

namespace Ui {
class UserDialog;
}

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);
    ~UserDialog();

private:
    Ui::UserDialog *ui;
    QStandardItemModel userData;
    void ExportTable();
    void UpdateUserTable();
    void OnDeleteUser();
    void OnAddUser();
    void OnModifyUser();
};
