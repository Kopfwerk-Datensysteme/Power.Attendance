#pragma once

#include "common.h"

static const QString SETTING_ADMIN_PASSWORD = "ADMIN_PASSWORD";
static const QString DEFAULT_ADMIN_PASSWORD = "1234";

struct User {
    QString biometricId;
    QString matriculationNumber;
    QString userName;
};

struct Attendance {
    QString biometricId;
    QString matriculationNumber;
    QString userName;
    QList<QDateTime> timestampValues;
};

void EventuallyHandleDatabaseError(bool success, QSqlQuery query);

void SetupSchemaIfNecessary();

void UpdateSetting(QString key, QString value);

QString GetSetting(QString key);

bool DoesUserExist(QString biometricId);

void CreateUser(User user);

void ModifyUser(User user);

void DeleteUser(QString biometricId);

User GetUser(QString biometricId);

QList<User> GetUsers(QString userName = "", QString matriculationNumber = "");

void AddTimestampForUser(QString biometricId);

QList<Attendance> GetAttendance(QDate fromDate, QDate toDate, QString userName = "", QString matriculationNumber = "");
