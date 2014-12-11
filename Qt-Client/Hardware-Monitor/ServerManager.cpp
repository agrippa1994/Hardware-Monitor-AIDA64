#include "ServerManager.h"
#include "ui_ServerManager.h"

ServerManager::ServerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManager)
{
    ui->setupUi(this);
}

ServerManager::~ServerManager()
{
    delete ui;
}
