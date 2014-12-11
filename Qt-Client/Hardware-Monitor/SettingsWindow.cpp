#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include <QDataStream>
#include <QFile>
#include <QMessageBox>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->okButton, SIGNAL(clicked()), SLOT(onOK()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()), SLOT(onCancel()));

    loadConfigurationFromFile();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

int SettingsWindow::timerInterval() const
{
    return ui->refreshRateSpinBox->value();
}

bool SettingsWindow::automaticallyConnect() const
{
    return ui->connectAutomaticallyCheckBox->isChecked();
}

QString SettingsWindow::serverIP() const
{
    return ui->serverIPLineEdit->text();
}

bool SettingsWindow::loadConfigurationFromFile()
{
    QFile file("configuration");
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream ds(file.readAll());

    int refreshRate = 0;
    bool connectAutomatically = false;
    QString serverIP;

    ds >> refreshRate >> connectAutomatically >> serverIP;

    ui->refreshRateSpinBox->setValue(refreshRate);
    ui->connectAutomaticallyCheckBox->setChecked(connectAutomatically);
    ui->serverIPLineEdit->setText(serverIP);

    return true;
}

bool SettingsWindow::saveConfigurationToFile()
{
    QFile file("configuration", this);
    if(!file.open(QFile::WriteOnly))
        return false;

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);

    ds << ui->refreshRateSpinBox->value() << ui->connectAutomaticallyCheckBox->isChecked() << ui->serverIPLineEdit->text();

    return file.write(data) > 0;
}

void SettingsWindow::onOK()
{
    if(!saveConfigurationToFile())
    {
        auto text = "The configuration file is not writable!\nDo you want to continue anyway?";
        if(QMessageBox::question(this, "Error", text) == QMessageBox::Yes)
            close();
    }
    else
    {
        close();
    }
}

void SettingsWindow::onCancel()
{
    close();
}
