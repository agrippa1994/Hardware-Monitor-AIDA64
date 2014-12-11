#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "aida64.h"

#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionSettings, SIGNAL(triggered()), &m_settingsWindow, SLOT(show()));
    QObject::connect(ui->actionServer_Manager, SIGNAL(triggered()), &m_serverManager, SLOT(show()));

    m_timerID = startTimer(m_settingsWindow.timerInterval());
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
