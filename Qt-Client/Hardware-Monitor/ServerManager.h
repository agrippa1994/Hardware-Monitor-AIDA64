#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QWidget>

namespace Ui {
class ServerManager;
}

class ServerManager : public QWidget
{
    Q_OBJECT

public:
    explicit ServerManager(QWidget *parent = 0);
    ~ServerManager();

private:
    Ui::ServerManager *ui;
};

#endif // SERVERMANAGER_H
