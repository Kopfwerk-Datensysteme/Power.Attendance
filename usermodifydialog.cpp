#include "usermodifydialog.h"
#include "ui_usermodifydialog.h"

UserModifyDialog::UserModifyDialog(User user, bool disableCancel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserModifyDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &UserModifyDialog::UpdateUser);
    // fill dialog fields
    ui->lineEditBiometricId->setText(user.biometricId);
    ui->lineEditMatriculationNumber->setText(user.matriculationNumber);
    ui->lineEditUserName->setText(user.userName);
    this->user = user;
    // remove help and close button in the top right corner
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
    // eventually disable the cancel button
    if (disableCancel) {
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    }
}

UserModifyDialog::~UserModifyDialog()
{
    delete ui;
}

void UserModifyDialog::UpdateUser() {
    user.biometricId = ui->lineEditBiometricId->text();
    user.matriculationNumber = ui->lineEditMatriculationNumber->text();
    user.userName = ui->lineEditUserName->text();
}
