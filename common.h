#pragma once

#include "stdint.h"
#include "stdio.h"
#include "conio.h"
#include "windows.h"
#include "memory.h"
#include "memory"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include "tuple"
#include "algorithm"
#include "functional"
#include <QMessageBox>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QMainWindow>
#include <QString>
#include <QException>
#include <QSqlError>
#include <QDateTime>
#include <QDate>
#include <QStandardItemModel>
#include <QPushButton>
#include <QDialog>
#include <QSet>
#include <QStandardPaths>
#include <QPicture>
#include <QThread>
#include <QInputDialog>

const QString TIME_FORMAT = "HH:mm";

void ShowMessage(QString message);

std::shared_ptr<QMessageBox> GetNonModalMessageBox(QString message);

bool CheckAdminPassword();
