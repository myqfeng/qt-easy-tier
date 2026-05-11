#ifndef QTEASYTIER_QTETSERVERSDIALOG_H
#define QTEASYTIER_QTETSERVERSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include "qtetcheckbtn.h"
#include "qtetpushbtn.h"

struct ServerInfoData
{
    QString name;
    QString address;
    bool isPublic = false;

    ServerInfoData() = default;
    ServerInfoData(QString n, QString a, bool pub = false)
        : name(std::move(n)), address(std::move(a)), isPublic(pub) {}

    static ServerInfoData fromJson(const QJsonObject &obj) {
        ServerInfoData info;
        // 兼容用户收藏格式 { "name": "...", "address": "..." }
        if (obj.contains("name") && obj.contains("address")) {
            info.name = obj["name"].toString();
            info.address = obj["address"].toString();
            info.isPublic = false;
        }
        // 兼容公共服务器格式 { "contributor": "...", "url": "..." }
        else if (obj.contains("contributor") && obj.contains("url")) {
            info.name = obj["contributor"].toString();
            info.address = obj["url"].toString();
            info.isPublic = true;
        }
        return info;
    }
};

class QtETServersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QtETServersDialog(QWidget *parent = nullptr);
    ~QtETServersDialog() override = default;

    void setSelectedServers(const QStringList &selectedAddresses);
    [[nodiscard]] QStringList selectedServers() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void loadServers();
    void loadPublicServers();
    void updateServerList();
    [[nodiscard]] QString serverFilePath() const;
    [[nodiscard]] QString publicServerFilePath() const;
    void updateColorScheme();
    void clearServerCheckboxes();

    QVBoxLayout *m_mainLayout = nullptr;
    QLabel *m_titleLabel = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContent = nullptr;
    QVBoxLayout *m_scrollLayout = nullptr;

    QtETPushBtn *m_selectAllBtn = nullptr;
    QtETPushBtn *m_deselectAllBtn = nullptr;
    QtETPushBtn *m_okBtn = nullptr;
    QtETPushBtn *m_cancelBtn = nullptr;

    QVector<ServerInfoData> m_servers;
    QList<QtETCheckBtn*> m_serverCheckBoxes;

    QStringList m_initiallySelectedAddresses;

    QColor m_bgColor;
    QColor m_borderColor;

private slots:
    void onSelectAll();
    void onDeselectAll();
};

#endif // QTEASYTIER_QTETSERVERSDIALOG_H
