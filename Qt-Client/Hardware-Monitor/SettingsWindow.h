#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

    int timerInterval() const;
    bool automaticallyConnect() const;
    QString serverIP() const;

    bool loadConfigurationFromFile();
    bool saveConfigurationToFile();

private:
    Ui::SettingsWindow *ui;

private slots:
    void onOK();
    void onCancel();

};

#endif // SETTINGSWINDOW_H
