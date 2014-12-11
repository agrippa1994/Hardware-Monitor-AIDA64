#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include "SettingsWindow.h"
#include "ServerManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void appendNetworkLog(const QString& s);
    void appendApplicationLog(const QString& s);

protected:
    virtual void timerEvent(QTimerEvent *);

private:
    int m_timerID = 0;

    Ui::MainWindow *ui;
    SettingsWindow m_settingsWindow;
    ServerManager m_serverManager;
};

#endif // MAINWINDOW_H
