#pragma once

#include "common.h"

namespace Ui {
class PasswordChangeDialog;
}

class PasswordChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordChangeDialog(QWidget *parent = nullptr);
    ~PasswordChangeDialog();
    QString newPassword;

private:
    Ui::PasswordChangeDialog *ui;
    void done(int r);
};
