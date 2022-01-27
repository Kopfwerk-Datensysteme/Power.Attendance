#pragma once

#include "common.h"
#include "fingerprintreader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FingerprintReader reader;

private slots:

    // user menu
    void on_actionModify_triggered();

    // attendance menu
    void on_actionCheck_triggered();
};
