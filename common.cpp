#include "common.h"

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
    return msgBox;
}
