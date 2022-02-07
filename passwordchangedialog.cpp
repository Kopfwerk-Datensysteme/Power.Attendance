#include "passwordchangedialog.h"
#include "ui_passwordchangedialog.h"
#include "database.h"

PasswordChangeDialog::PasswordChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordChangeDialog)
{
    ui->setupUi(this);
}

PasswordChangeDialog::~PasswordChangeDialog()
{
    delete ui;
}

void PasswordChangeDialog::done(int r)
{
    if(QDialog::Accepted == r)
    {
        if (ui->lineEditNewPassword->text() != ui->lineEditNewPasswordRepetition->text()) {
            ShowMessage("Die beiden neuen Passwörter stimmen nicht überein!");
            return;
        }
        // set the new password in the member variable
        newPassword = ui->lineEditNewPassword->text();
    }
    QDialog::done(r);
}
