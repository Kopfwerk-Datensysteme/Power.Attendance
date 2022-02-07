#include "common.h"
#include "database.h"

void ShowMessage(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

std::shared_ptr<QMessageBox> GetNonModalMessageBox(QString message) {
    auto msgBox = std::make_shared<QMessageBox>();
    msgBox->setText(message);
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setStandardButtons(QMessageBox::NoButton);
    msgBox->show();
    msgBox->raise();
    QCoreApplication::processEvents();
    return msgBox;
}

bool CheckAdminPassword() {
    QInputDialog dlg;
    dlg.setWindowTitle("Admin-Passwort");
    dlg.setLabelText("Admin-Passwort eingeben:");
    dlg.setTextEchoMode(QLineEdit::Password);
    if (dlg.exec() == QDialog::Accepted) {
        QString currentPassword;
        try {
            currentPassword = GetSetting(SETTING_ADMIN_PASSWORD);
        } catch (QException e) {
            ShowMessage("Das aktuelle Passwort konnte nicht aus der Datenbank ausgelesen werden!");
            return false;
        }
        QString dialogPassword = dlg.textValue();
        if (dialogPassword == currentPassword) {
            return true;
        } else {
            ShowMessage("Sie haben das falsche Passwort eingegeben!");
            return false;
        }
    }
    return false;
}
