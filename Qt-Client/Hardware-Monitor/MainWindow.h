#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QWebSocket>
#include <QHideEvent>
#include <QShowEvent>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SettingsWindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void appendNetworkLog(const QString& s);
    void appendApplicationLog(const QString& s);

protected:
    virtual void timerEvent(QTimerEvent *);
    virtual void hideEvent(QHideEvent *p);
    virtual void showEvent(QShowEvent *p);

private:
    int m_timerID = 0;

    Ui::MainWindow *ui;
    SettingsWindow m_settingsWindow;
    QWebSocket m_webSocket;
    QSystemTrayIcon m_systemTrayIcon;
    QMenu m_systemTrayIconMenu;

private slots:
    void stateChanged(QAbstractSocket::SocketState state);
    void onConnect();
    void onDisconnect();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void trayMenuTriggered(QAction *action);
};

#endif // MAINWINDOW_H
