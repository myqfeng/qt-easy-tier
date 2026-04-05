#ifndef PUBLICSERVER_H
#define PUBLICSERVER_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>

namespace Ui {
class PublicServer;
}

class PublicServer : public QDialog
{
    Q_OBJECT

public:
    explicit PublicServer(QWidget *parent = nullptr);
    ~PublicServer();

    // 返回选中的服务器地址列表
    QStringList getSelectedServers();

private slots:
    void onTableCellClicked(int row, int column);

private:
    Ui::PublicServer *ui;
    QJsonArray serverData;
    QStringList selectedUrls; // 存储已选择的URL

    void loadServerData();
    void populateTable();
    void toggleUrlSelection(const QString &url);
    bool isUrlSelected(const QString &url) const;
};

#endif // PUBLICSERVER_H
