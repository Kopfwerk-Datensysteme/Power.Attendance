#pragma once

#include "common.h"

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

private slots:

    // user menu
    void OnModify();

    // attendance menu
    void OnCheck();

    // add timestamp
    void OnAddTimestamp();
};
