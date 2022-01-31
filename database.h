#pragma once

#include "common.h"

struct User {
    QString biometricId;
    QString matriculationNumber;
    QString userName;
};

struct Timestamp {
    QString biometricId;
    QString matriculationNumber;
    QString userName;
    qint64 timestampValue;
};

void EventuallyHandleDatabaseError(bool success, QSqlQuery query);

void SetupSchemaIfNecessary();

bool DoesUserExist(QString biometricId);

void CreateUser(User user);

void ModifyUser(User user);

void DeleteUser(QString biometricId);

QList<User> GetAllUsers();

void AddTimestampForUser(QString biometricId);

QList<Timestamp> GetAllTimestampsForDateUserNameMatriculationNumber(QDate date, QString userName, QString matriculationNumber);
