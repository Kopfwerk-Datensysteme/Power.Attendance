#include "usermodifydialog.h"
#include "ui_usermodifydialog.h"
#include "database.h"

UserModifyDialog::UserModifyDialog(User user, bool newUser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserModifyDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, &UserModifyDialog::UpdateUser);
    // save initial values
    initialMatriculationNumber = user.matriculationNumber;
    // fill dialog fields
    ui->lineEditBiometricId->setText(user.biometricId);
    ui->lineEditMatriculationNumber->setText(user.matriculationNumber);
    ui->lineEditUserName->setText(user.userName);
    this->user = user;
    this->newUser = newUser;
    // remove help and close button in the top right corner
    this->setWindowFlags(Qt::Dialog | Qt::Desktop);
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
