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
        QString currentPassword;
        try {
            currentPassword = GetSetting(SETTING_ADMIN_PASSWORD);
        } catch (QException e) {
            ShowMessage("Das aktuelle Passwort konnte nicht aus der Datenbank ausgelesen werden!");
            return;
        }
        if (ui->lineEditOldPassword->text() != currentPassword)
        {
            ShowMessage("Das eingegebene alte Passwort stimmt nicht mit dem aktuellen Passwort überein!");
            return;
        }
        if (ui->lineEditNewPassword->text() != ui->lineEditNewPasswordRepetition->text()) {
            ShowMessage("Die beiden neuen Passwörter stimmen nicht überein!");
            return;
        }
        // set the new password in the member variable
        newPassword = ui->lineEditNewPassword->text();
    }
    QDialog::done(r);
}
