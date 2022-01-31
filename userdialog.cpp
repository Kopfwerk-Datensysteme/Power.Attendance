#include "userdialog.h"
#include "ui_userdialog.h"
#include "usermodifydialog.h"
#include "database.h"

UserDialog::UserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);
    // connect slots
    connect(ui->userModifyButton, &QPushButton::pressed, this, &UserDialog::OnModifyUser);
    connect(ui->userDeleteButton, &QPushButton::pressed, this, &UserDialog::OnDeleteUser);
    connect(ui->lineEditUserName, &QLineEdit::textChanged, this, &UserDialog::UpdateUserTable);
    connect(ui->lineEditMatriculationNumber, &QLineEdit::textChanged, this, &UserDialog::UpdateUserTable);
    // set up user table
    ui->userTable->setModel(&userData);
    ui->userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    UpdateUserTable();
}

UserDialog::~UserDialog()
{
    delete ui;
}

void UserDialog::UpdateUserTable() {
    QList<User> userList;
    try {
        userList = GetUsers(ui->lineEditUserName->text(), ui->lineEditMatriculationNumber->text());
    } catch (QException e) {
        ShowMessage("Die Benutzer konnten nicht von der Datenbank geladen werden!");
    }
    userData.clear();
    userData.setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
    userData.setHorizontalHeaderItem(1, new QStandardItem(QString("Matrikelnummer")));
    userData.setHorizontalHeaderItem(2, new QStandardItem(QString("Name")));
    for (int row = 0; row < userList.size(); row++) {
        QList<QStandardItem*> itemList;
        itemList.append(new QStandardItem(userList[row].biometricId));
        itemList.append(new QStandardItem(userList[row].matriculationNumber));
        itemList.append(new QStandardItem(userList[row].userName));
        userData.appendRow(itemList);
    }
    ui->userTable->selectRow(0);
    ui->userModifyButton->setEnabled(0 < userList.size());
    ui->userDeleteButton->setEnabled(0 < userList.size());
    ui->userTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void UserDialog::OnDeleteUser() {
    qint64 selectedRow = ui->userTable->selectionModel()->selectedRows()[0].row();
    QString biometricId = userData.item(selectedRow)->text();
    try {
        DeleteUser(biometricId);
    } catch (QException e) {
        ShowMessage("Der Benutzer konnte nicht gelöscht werden!");
    }
    UpdateUserTable();
}

void UserDialog::OnModifyUser() {
    qint64 selectedRow = ui->userTable->selectionModel()->selectedRows()[0].row();
    QString biometricId = userData.item(selectedRow, 0)->text();
    QString matriculationNumber = userData.item(selectedRow, 1)->text();
    QString userName = userData.item(selectedRow, 2)->text();
    UserModifyDialog dlg({biometricId, matriculationNumber, userName});
    if (dlg.exec() == QDialog::Accepted) {
        try {
            ModifyUser(dlg.user);
        } catch (QException e) {
            ShowMessage("Der Benutzer konnte nicht geändert werden!");
        }
    }
    UpdateUserTable();
}
