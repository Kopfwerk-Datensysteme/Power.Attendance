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
    success = query.exec("PRAGMA foreign_keys = ON;");
    EventuallyHandleDatabaseError(success, query);
    success = query.exec("CREATE TABLE IF NOT EXISTS Users (matriculationNumber text NOT NULL, biometricId text NOT NULL UNIQUE, userName text NOT NULL, PRIMARY KEY (matriculationNumber));");
    EventuallyHandleDatabaseError(success, query);
    success = query.exec("CREATE TABLE IF NOT EXISTS Timestamps (matriculationNumber text NOT NULL, timestampValue integer NOT NULL, FOREIGN KEY (matriculationNumber) REFERENCES Users(matriculationNumber) ON DELETE CASCADE ON UPDATE CASCADE);");
    EventuallyHandleDatabaseError(success, query);
    success = query.exec("CREATE TABLE IF NOT EXISTS Settings (key text NOT NULL, value text NOT NULL, PRIMARY KEY (key));");
    EventuallyHandleDatabaseError(success, query);
    success = query.exec("INSERT OR IGNORE INTO Settings (key, value) VALUES ('" + SETTING_ADMIN_PASSWORD + "', '" + DEFAULT_ADMIN_PASSWORD + "');");
    EventuallyHandleDatabaseError(success, query);
}

void UpdateSetting(QString key, QString value) {
    QSqlQuery query;
    bool success;
    success = query.prepare("UPDATE Settings SET value = ? WHERE key = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(value);
    query.addBindValue(key);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

QString GetSetting(QString key) {
    QSqlQuery query;
    QString value;
    bool success;
    success = query.prepare("SELECT * FROM Settings WHERE key = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(key);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    qint64 returnedRows = 0;
    while (query.next()) {
        returnedRows++;
        value = query.value(1).toString();
    }
    if (returnedRows != 1) {
        throw QException();
    }
    return value;
}

bool DoesUserWithBiometricIdExist(QString biometricId) {
    try {
        User user = GetUserWithBiometricId(biometricId);
    } catch (QException e) {
        return false;
    }
    return true;
}

bool DoesUserWithMatriculationNumberExist(QString matriculationNumber) {
    try {
        User user = GetUserWithMatriculationNumber(matriculationNumber);
    } catch (QException e) {
        return false;
    }
    return true;
}

void CreateUser(User user) {
    QSqlQuery query;
    bool success;
    success = query.prepare("INSERT INTO Users (matriculationNumber, biometricId, userName) VALUES (?, ?, ?);");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(user.matriculationNumber);
    query.addBindValue(user.biometricId);
    query.addBindValue(user.userName);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

void ModifyUser(User user, QString matriculationNumber) {
    QSqlQuery query;
    bool success;
    success = query.prepare("UPDATE Users SET matriculationNumber = ?, biometricId = ?, userName = ? WHERE matriculationNumber = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(user.matriculationNumber);
    query.addBindValue(user.biometricId);
    query.addBindValue(user.userName);
    query.addBindValue(matriculationNumber);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

void DeleteUser(QString column, QString value) {
    QSqlQuery query;
    bool success;
    success = query.prepare("DELETE FROM Users WHERE " + column + " = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(value);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

void DeleteUserWithBiometricId(QString biometricId) {
    DeleteUser("biometricId", biometricId);
}

void DeleteUserWithMatriculationNumber(QString matriculationNumber) {
    DeleteUser("matriculationNumber", matriculationNumber);
}

User GetUser(QString column, QString value) {
    QSqlQuery query;
    User user;
    bool success;
    success = query.prepare("SELECT * FROM Users WHERE " + column + " = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(value);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    qint64 returnedRows = 0;
    while (query.next()) {
        returnedRows++;
        user = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString()};
    }
    if (returnedRows != 1) {
        throw QException();
    }
    return user;
}

User GetUserWithBiometricId(QString biometricId) {
    return GetUser("biometricId", biometricId);
}

User GetUserWithMatriculationNumber(QString matriculationNumber) {
    return GetUser("matriculationNumber", matriculationNumber);
}

QList<User> GetUsers(QString userName, QString matriculationNumber) {
    QSqlQuery query;
    bool success;
    success = query.prepare("SELECT * FROM Users WHERE userName LIKE ('%' || ? || '%') AND matriculationNumber LIKE ('%' || ? || '%');");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(userName);
    query.addBindValue(matriculationNumber);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    QList<User> userList;
    while (query.next()) {
        User currentUser = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString()};
        userList.append(currentUser);
    }
    return userList;
}

void AddTimestampForUserWithBiometricId(QString biometricId) {
    User user = GetUserWithBiometricId(biometricId);
    AddTimestampForUserWithMatriculationNumber(user.matriculationNumber);
}

void AddTimestampForUserWithMatriculationNumber(QString matriculationNumber) {
    QSqlQuery query;
    bool success;
    success = query.prepare("INSERT INTO Timestamps (matriculationNumber, timestampValue) VALUES (?, ?);");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(matriculationNumber);
    qint64 timestampValue = QDateTime::currentDateTime().toSecsSinceEpoch();
    query.addBindValue(timestampValue);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
}

QList<Attendance> GetAttendances(QDate fromDate, QDate toDate, QString userName, QString matriculationNumber) {
    qint64 startOfDayTimestampValue;
    if (fromDate.isValid()) {
        startOfDayTimestampValue = fromDate.startOfDay().toSecsSinceEpoch();
    } else {
        startOfDayTimestampValue = 0;
    }
    qint64 endOfDayTimestampValue;
    if (toDate.isValid()) {
        endOfDayTimestampValue = toDate.endOfDay().toSecsSinceEpoch();
    } else {
        endOfDayTimestampValue = std::numeric_limits<qint64>::max();
    }
    QSqlQuery query;
    bool success;
    success = query.prepare("SELECT *, GROUP_CONCAT(timestampValue) FROM Timestamps INNER JOIN Users ON Timestamps.matriculationNumber = Users.matriculationNumber WHERE ? <= timestampValue AND timestampValue <= ? AND userName LIKE ('%' || ? || '%') AND Users.matriculationNumber LIKE ('%' || ? || '%') GROUP BY Users.matriculationNumber;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(startOfDayTimestampValue);
    query.addBindValue(endOfDayTimestampValue);
    query.addBindValue(userName);
    query.addBindValue(matriculationNumber);
    success = query.exec();
    EventuallyHandleDatabaseError(success, query);
    QList<Attendance> attendanceList;
    while (query.next()) {
        QList<QDateTime> timestampList;
        for (QString& timestampString : query.value(5).toString().split(",")) {
            timestampList.append(QDateTime::fromSecsSinceEpoch(timestampString.toLongLong()));
        }
        std::sort(timestampList.begin(), timestampList.end());
        Attendance currentAttendance = {query.value(0).toString(), query.value(3).toString(), query.value(4).toString(), timestampList};
        attendanceList.append(currentAttendance);
    }
    return attendanceList;
}
