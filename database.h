#pragma once

#include "common.h"

static const QString SETTING_ADMIN_PASSWORD = "ADMIN_PASSWORD";
static const QString DEFAULT_ADMIN_PASSWORD = "1234";

struct User {
    QString matriculationNumber;
    QString biometricId;
    QString userName;
};

struct Attendance {
    QString matriculationNumber;
    QString biometricId;
    QString userName;
    QList<QDateTime> timestampValues;
};

void EventuallyHandleDatabaseError(bool success, QSqlQuery query);

void SetupSchemaIfNecessary();

void UpdateSetting(QString key, QString value);

QString GetSetting(QString key);

bool DoesUserWithBiometricIdExist(QString biometricId);

bool DoesUserWithMatriculationNumberExist(QString matriculationNumber);

void CreateUser(User user);

void ModifyUser(User user, QString matriculationNumber);

void DeleteUserWithBiometricId(QString biometricId);

void DeleteUserWithMatriculationNumber(QString matriculationNumber);

User GetUserWithBiometricId(QString biometricId);

User GetUserWithMatriculationNumber(QString matriculationNumber);

QList<User> GetUsers(QString userName = "", QString matriculationNumber = "");

void AddTimestampForUserWithBiometricId(QString biometricId);

void AddTimestampForUserWithMatriculationNumber(QString matriculationNumber);

QList<Attendance> GetAttendances(QDate fromDate, QDate toDate, QString userName = "", QString matriculationNumber = "");
