#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "aida64.h"

#include <QMessageBox>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_systemTrayIcon(this)
{
    ui->setupUi(this);

    QObject::connect(ui->actionSettings, SIGNAL(triggered()), &m_settingsWindow, SLOT(show()));
    QObject::connect(&m_webSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
    QObject::connect(ui->actionConnect, SIGNAL(triggered()), SLOT(onConnect()));
    QObject::connect(ui->actionDisconnect, SIGNAL(triggered()), SLOT(onDisconnect()));
    QObject::connect(&m_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    m_timerID = startTimer(m_settingsWindow.timerInterval());

    if(m_settingsWindow.automaticallyConnect())
        m_webSocket.open(QUrl(m_settingsWindow.serverIP()));

    m_systemTrayIcon.setVisible(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendNetworkLog(const QString &s)
{
    auto date = QDateTime::currentDateTime().toString("[dd:MM:yyyy hh:mm:ss:zzz] ");
    ui->networkLog->append(date + s);
}

void MainWindow::appendApplicationLog(const QString &s)
{
    auto date = QDateTime::currentDateTime().toString("[dd:MM:yyyy hh:mm:ss:zzz] ");
    ui->applicationLog->append(date + s);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    try
    {
        using namespace aida64;
        API api;
        if(!api.refreshData())
            return appendApplicationLog("Refreshing API-Data failed");

        const auto& data = api.data();

        // Update table content
        {
            if(data.size() != ui->mainDataTable->rowCount())
            {
                appendApplicationLog("(Re-)loading main table");
                for(int i = 0; i < ui->mainDataTable->rowCount(); i++)
                    ui->mainDataTable->removeRow(i);

                ui->mainDataTable->setRowCount(data.size());

                int idx = 0;
                for(const auto& i : data)
                {
                    auto category = new QTableWidgetItem(QString(i.category.c_str()));
                    auto id = new QTableWidgetItem(QString(i.id.c_str()));
                    auto label = new QTableWidgetItem(QString(i.label.c_str()));
                    auto value = new QTableWidgetItem(QString(i.value.c_str()));

                    ui->mainDataTable->setItem(idx, 0, category);
                    ui->mainDataTable->setItem(idx, 1, id);
                    ui->mainDataTable->setItem(idx, 2, label);
                    ui->mainDataTable->setItem(idx, 3, value);

                    idx++;
                }
            }
            else
            {
                int idx = 0;
                for(const auto& i : data)
                {
                    ui->mainDataTable->item(idx, 0)->setText(i.category.c_str());
                    ui->mainDataTable->item(idx, 1)->setText(i.id.c_str());
                    ui->mainDataTable->item(idx, 2)->setText(i.label.c_str());
                    ui->mainDataTable->item(idx, 3)->setText(i.value.c_str());

                    idx++;
                }
            }
        }

        // Send data to server
        {
            if(m_webSocket.state() == QAbstractSocket::ConnectedState)
            {
                QJsonObject object;

                QJsonArray array;
                for(const auto& i : data)
                {
                    QJsonObject sensorValue;
                    sensorValue["category"] = i.category.c_str();
                    sensorValue["id"] = i.id.c_str();
                    sensorValue["label"] = i.label.c_str();
                    sensorValue["value"] = i.value.c_str();

                    array.push_back(sensorValue);
                }

                object["computerName"] = api.computerName().c_str();
                object["sensorValues"] = array;

                auto bytes = m_webSocket.sendTextMessage(QString(QJsonDocument(object).toJson()));
                appendNetworkLog(QString().sprintf("Bytes sent: %d", bytes));
            }
        }

        appendApplicationLog("Data successfully updated");

        killTimer(m_timerID);
        m_timerID = startTimer(m_settingsWindow.timerInterval());

    }
    catch(const std::exception& e)
    {
        auto str = QString("An exception occured while fetching AIDA-Data: ") + QString(e.what());
        appendApplicationLog(str);
    }

    catch(...)
    {
        appendApplicationLog("An unknown exception occured while fetching AIDA-Data");
    }
}

void MainWindow::hideEvent(QHideEvent *p)
{
    if(isMinimized())
    {
        ::ShowWindow((HWND)winId(), SW_HIDE);
    }

    QWidget::hideEvent(p);
}

void MainWindow::showEvent(QShowEvent *p)
{
    QWidget::showEvent(p);

    ::ShowWindow((HWND)winId(), SW_SHOW);
}

void MainWindow::stateChanged(QAbstractSocket::SocketState state)
{
    QString text;
    switch(state)
    {
    case QAbstractSocket::UnconnectedState:
        text = "The WebSocket is not connected";
        break;
    case QAbstractSocket::HostLookupState:
        text = "The WebSocket is performing a host name lookup";
        break;
    case QAbstractSocket::ConnectingState:
        text = "The WebSocket has started establishing a connection";
        break;
    case QAbstractSocket::ConnectedState:
        text = "The WebSocket connection is established";
        break;
    case QAbstractSocket::ClosingState:
        text = "The WebSocket is about to close (data may still be waiting to be written)";
        break;
    default:
        text = "Error";
        break;
    }

    appendNetworkLog(text);
}

void MainWindow::onConnect()
{
    auto ip = m_settingsWindow.serverIP();
    if(!ip.length())
    {
        QMessageBox::warning(this, "Warning", "The IP is invalid", "OK");
        m_settingsWindow.show();
        return;
    }

    m_webSocket.open(QUrl(ip));
}

void MainWindow::onDisconnect()
{
    m_webSocket.close();
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(isMinimized())
        {
            ::ShowWindow((HWND)winId(), SW_SHOW);
            showNormal();
        }
        else
        {
            showMinimized();
            ::ShowWindow((HWND)winId(), SW_HIDE);
        }
    break;
    }
}

