#include "database.h"

void EventuallyHandleDatabaseError(bool success, QSqlQuery query) {
    if (!success) {
        ShowMessage(query.lastError().text());
        throw QException();
    }
}

void SetupSchemaIfNecessary() {
    QSqlQuery query;
    bool success;
    success = query.exec("CREATE TABLE IF NOT EXISTS Users (biometricId text NOT NULL, matriculationNumber text NOT NULL, userName text NOT NULL, PRIMARY KEY (biometricId));");
    EventuallyHandleDatabaseError(success, query);
    success = query.exec("CREATE TABLE IF NOT EXISTS Timestamps (biometricId text NOT NULL, timestampValue integer NOT NULL, FOREIGN KEY (biometricId) REFERENCES Users(biometricId));");
    EventuallyHandleDatabaseError(success, query);
}

bool DoesUserExist(QString biometricId) {
    QSqlQuery query;
    bool success;
    success = query.prepare("SELECT * FROM Users WHERE biometricId = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(biometricId);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    qint64 returnedRows = 0;
    while (query.next()) {
        returnedRows++;
    }
    return returnedRows == 1;
}

void CreateUser(User user) {
    QSqlQuery query;
    bool success;
    success = query.prepare("INSERT INTO Users (biometricId, matriculationNumber, userName) VALUES (?, ?, ?);");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(user.biometricId);
    query.addBindValue(user.matriculationNumber);
    query.addBindValue(user.userName);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

void ModifyUser(User user) {
    QSqlQuery query;
    bool success;
    success = query.prepare("UPDATE Users SET matriculationNumber = ?, userName = ? WHERE biometricId = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(user.matriculationNumber);
    query.addBindValue(user.userName);
    query.addBindValue(user.biometricId);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

void DeleteUser(QString biometricId) {
    QSqlQuery query;
    bool success;
    success = query.prepare("DELETE FROM Users WHERE biometricId = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(biometricId);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

QList<User> GetAllUsers() {
    QSqlQuery query;
    bool success;
    success = query.exec("SELECT * FROM Users;");
    EventuallyHandleDatabaseError(success, query);
    QList<User> userList;
    while (query.next()) {
        User currentUser = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString()};
        userList.append(currentUser);
    }
    return userList;
}

void AddTimestampForUser(QString biometricId) {
    QSqlQuery query;
    bool success;
    success = query.prepare("INSERT INTO Timestamps (biometricId, timestampValue) VALUES (?, ?);");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(biometricId);
    qint64 timestampValue = QDateTime::currentDateTime().toSecsSinceEpoch();
    query.addBindValue(timestampValue);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

QList<Timestamp> GetAllTimestampsForDateUserNameMatriculationNumber(QDate date, QString userName, QString matriculationNumber) {
    qint64 startOfDayTimestampValue = date.startOfDay().toSecsSinceEpoch();
    qint64 endOfDayTimestampValue = date.endOfDay().toSecsSinceEpoch();
    QSqlQuery query;
    bool success;
    success = query.prepare("SELECT * FROM Timestamps INNER JOIN Users ON Timestamps.biometricId = Users.biometricId WHERE ? <= timestampValue AND timestampValue <= ? AND userName LIKE ('%' || ? || '%') AND matriculationNumber LIKE ('%' || ? || '%');");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(startOfDayTimestampValue);
    query.addBindValue(endOfDayTimestampValue);
    query.addBindValue(userName);
    query.addBindValue(matriculationNumber);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    QList<Timestamp> timestampList;
    while (query.next()) {
        Timestamp currentTimestamp = {query.value(0).toString(), query.value(3).toString(), query.value(4).toString(), query.value(1).toLongLong()};
        timestampList.append(currentTimestamp);
    }
    return timestampList;
}
