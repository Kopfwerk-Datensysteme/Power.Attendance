#include "common.h"
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[])
{
    // setup high dpi mode
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // setup application
    QApplication app(argc, argv);
    app.setOrganizationName("KOPFWERK");
    app.setApplicationName ("Power.Attendance");
    app.setWindowIcon(QIcon(":/img/icon.png"));
    // show main window
    MainWindow w;
    w.show();
    // open and set up database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString databasePath = QCoreApplication::applicationDirPath() + QDir::separator() + "database.db";
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        ShowMessage("Die Datenbank konnte nicht geöffnet werden!");
        return -1;
    }
    try {
        SetupSchemaIfNecessary();
    } catch (QException e) {
        ShowMessage("Das Datenbank-Schema konnte nicht erstellt werden!");
        return -1;
    }
    // start app
    return app.exec();
}
