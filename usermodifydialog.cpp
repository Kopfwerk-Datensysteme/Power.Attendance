#include "usermodifydialog.h"
#include "ui_usermodifydialog.h"
#include "database.h"
#include "fingerprintreader.h"

UserModifyDialog::UserModifyDialog(User user, bool newUser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserModifyDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &UserModifyDialog::UpdateUser);
    connect(ui->pushButtonNewFingerprint, &QPushButton::pressed, this, &UserModifyDialog::GetNewBiometricId);
    // save initial values
    initialMatriculationNumber = user.matriculationNumber;
    // fill dialog fields
    ui->lineEditBiometricId->setText(user.biometricId);
    ui->lineEditMatriculationNumber->setText(user.matriculationNumber);
    ui->lineEditUserName->setText(user.userName);
    this->user = user;
    this->newUser = newUser;
    // disable new fingerprint button
    if (this->newUser) {
        ui->pushButtonNewFingerprint->setVisible(false);
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

void UserModifyDialog::done(int r)
{
    if(QDialog::Accepted == r)
    {
        QString currentMatriculationNumber = ui->lineEditMatriculationNumber->text();
        bool matriculationNumberChanged = currentMatriculationNumber != initialMatriculationNumber;
        if ((newUser || (!newUser && matriculationNumberChanged)) && DoesUserWithMatriculationNumberExist(currentMatriculationNumber)) {
            ShowMessage("Ein Benutzer mit dieser Matrikelnummer ist bereits in der Datenbank vorhanden!");
            return;
        }
    }
    QDialog::done(r);
}

void UserModifyDialog::GetNewBiometricId() {
    try {
        ui->lineEditBiometricId->setText(RegisterBiometricIdForFingerprint());
        ui->pushButtonNewFingerprint->setEnabled(false);
        UpdateUser();
    } catch (QException e) {
        ShowMessage("Der Fingerabdruck konnte nicht in der Datenbank angelegt werden!");
    }
}
