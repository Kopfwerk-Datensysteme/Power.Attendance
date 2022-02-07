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
#include <QFileDialog>
#include <QTextStream>
#include <QFile>

const QString TIME_FORMAT = "HH:mm";

void ShowMessage(QString message);

std::shared_ptr<QMessageBox> GetNonModalMessageBox(QString message);

bool CheckAdminPassword();

QString GetCSVField(QString text);

QString ConvertModelToCSV(QStandardItemModel& data);

void SaveContentToFile(QString content, QString fileName, QString caption = "Datei speichern ...", QString directory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
