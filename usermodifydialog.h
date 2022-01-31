#pragma once

#include "common.h"
#include "database.h"

namespace Ui {
class UserModifyDialog;
}

class UserModifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserModifyDialog(User user, QWidget *parent = nullptr);
    ~UserModifyDialog();
    User user;

private:
    Ui::UserModifyDialog *ui;
    void UpdateUser();
};