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

bool DoesUserExist(QString biometricId) {
    try {
        User user = GetUser(biometricId);
    } catch (QException e) {
        return false;
    }
    return true;
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

User GetUser(QString biometricId) {
    QSqlQuery query;
    User user;
    bool success;
    success = query.prepare("SELECT * FROM Users WHERE biometricId = ?;");
    EventuallyHandleDatabaseError(success, query);
    query.addBindValue(biometricId);
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

QList<Attendance> GetAttendance(QDate date, QString userName, QString matriculationNumber) {
    qint64 startOfDayTimestampValue = date.startOfDay().toSecsSinceEpoch();
    qint64 endOfDayTimestampValue = date.endOfDay().toSecsSinceEpoch();
    QSqlQuery query;
    bool success;
    success = query.prepare("SELECT *, GROUP_CONCAT(timestampValue) FROM Timestamps INNER JOIN Users ON Timestamps.biometricId = Users.biometricId WHERE ? <= timestampValue AND timestampValue <= ? AND userName LIKE ('%' || ? || '%') AND matriculationNumber LIKE ('%' || ? || '%') GROUP BY Users.biometricId;");
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
        Attendance currentAttendance = {query.value(0).toString(), query.value(3).toString(), query.value(4).toString(), timestampList};
        attendanceList.append(currentAttendance);
    }
    return attendanceList;
}
