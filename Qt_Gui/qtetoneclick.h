#ifndef QTETONECLICK_H
#define QTETONECLICK_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include "qtetcheckbtn.h"

class QtETOneClick : public QWidget
{
    Q_OBJECT

public:
    explicit QtETOneClick(QWidget *parent = nullptr);
    ~QtETOneClick() override;

private:
    /// @brief 初始化界面
    void initUI();
    /// @brief 初始化标题区域
    void initTitleArea();
    /// @brief 初始化表单区域
    void initFormArea();
    /// @brief 初始化服务器地址区域
    void initServerArea();
    /// @brief 初始化底部按钮区域
    void initButtonArea();
    /// @brief 设置信号连接
    void setupConnections();

    // 主布局
    QVBoxLayout *m_mainLayout = nullptr;

    // 标题区域
    QWidget *m_titleWidget = nullptr;
    QLabel *m_iconLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_rightIconLabel = nullptr;

    // 表单区域
    QWidget *m_formWidget = nullptr;
    QLineEdit *m_roomIdEdit = nullptr;      ///< 联机房间号输入框
    QLineEdit *m_hostIpEdit = nullptr;      ///< 房主虚拟 IP 输入框

    // 一键联机按钮
    QPushButton *m_oneClickBtn = nullptr;

    // 服务器地址区域
    QWidget *m_serverWidget = nullptr;
    QLineEdit *m_serverEdit = nullptr;      ///< 服务器地址输入框
    QPushButton *m_addServerBtn = nullptr;  ///< 添加服务器按钮
    QListWidget *m_serverListWidget = nullptr; ///< 服务器列表
    QPushButton *m_removeServerBtn = nullptr;  ///< 删除服务器按钮
    QPushButton *m_publicServerBtn = nullptr;  ///< 公共服务器列表按钮

    // 底部按钮区域
    QWidget *m_bottomWidget = nullptr;
    QtETCheckBtn *m_hostModeCheckBox = nullptr;  ///< 我做房主开关
    QtETCheckBtn *m_latencyFirstCheckBox = nullptr; ///< 低延迟优先开关

private slots:
    /// @brief 添加服务器按钮点击
    void onAddServer();
    /// @brief 删除服务器按钮点击
    void onRemoveServer();
    /// @brief 服务器列表选中项变化
    void onServerSelectionChanged();
    /// @brief 服务器地址输入框回车
    void onServerEditReturnPressed();
};

#endif // QTETONECLICK_H