#include "qtetoneclick.h"
#include <QFont>

QtETOneClick::QtETOneClick(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

QtETOneClick::~QtETOneClick() = default;

void QtETOneClick::initUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    initTitleArea();
    initFormArea();
    initServerArea();
    initButtonArea();

    setupConnections();
}

void QtETOneClick::initTitleArea()
{
    m_titleWidget = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleWidget);
    titleLayout->setSpacing(15);
    titleLayout->setContentsMargins(16, 15, 16, 6);

    // 左侧图标
    m_iconLabel = new QLabel(m_titleWidget);
    m_iconLabel->setMaximumSize(48, 48);
    m_iconLabel->setPixmap(QPixmap(":/icons/icon.ico"));
    m_iconLabel->setScaledContents(true);

    // 标题文字
    m_titleLabel = new QLabel(m_titleWidget);
    QFont titleFont;
    titleFont.setPointSize(20);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setText(QStringLiteral("一键联机"));
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // 右侧图标
    m_rightIconLabel = new QLabel(m_titleWidget);
    m_rightIconLabel->setMaximumSize(48, 48);
    m_rightIconLabel->setPixmap(QPixmap(":/icons/one-click.svg"));
    m_rightIconLabel->setScaledContents(true);

    titleLayout->addWidget(m_iconLabel, 0, Qt::AlignTop);
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    titleLayout->addWidget(m_rightIconLabel);

    m_mainLayout->addWidget(m_titleWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
}

void QtETOneClick::initFormArea()
{
    m_formWidget = new QWidget(this);
    QFormLayout *formLayout = new QFormLayout(m_formWidget);

    QFont labelFont;
    labelFont.setPointSize(12);

    // 联机房间号
    QLabel *roomIdLabel = new QLabel(m_formWidget);
    roomIdLabel->setFont(labelFont);
    roomIdLabel->setText(QStringLiteral("联机房间号："));

    m_roomIdEdit = new QLineEdit(m_formWidget);
    m_roomIdEdit->setFont(labelFont);
    m_roomIdEdit->setAlignment(Qt::AlignCenter);  // 文字居中

    // 房主虚拟 IP
    QLabel *hostIpLabel = new QLabel(m_formWidget);
    hostIpLabel->setFont(labelFont);
    hostIpLabel->setText(QStringLiteral("房主虚拟IP："));

    m_hostIpEdit = new QLineEdit(m_formWidget);
    m_hostIpEdit->setFont(labelFont);
    m_hostIpEdit->setAlignment(Qt::AlignCenter);  // 文字居中

    formLayout->addRow(roomIdLabel, m_roomIdEdit);
    formLayout->addRow(hostIpLabel, m_hostIpEdit);

    m_mainLayout->addWidget(m_formWidget, 0, Qt::AlignTop);

    // 一键联机按钮
    m_oneClickBtn = new QPushButton(this);
    m_oneClickBtn->setMinimumWidth(200);
    m_oneClickBtn->setMaximumWidth(200);
    QFont btnFont;
    btnFont.setPointSize(12);
    m_oneClickBtn->setFont(btnFont);
    m_oneClickBtn->setText(QStringLiteral("一键联机"));

    m_mainLayout->addWidget(m_oneClickBtn, 0, Qt::AlignHCenter);
}

void QtETOneClick::initServerArea()
{
    m_serverWidget = new QWidget(this);
    QGridLayout *serverLayout = new QGridLayout(m_serverWidget);

    // 服务器地址标签
    QLabel *serverLabel = new QLabel(m_serverWidget);
    QFont labelFont;
    labelFont.setPointSize(11);
    serverLabel->setFont(labelFont);
    serverLabel->setText(QStringLiteral("服务器地址："));

    // 服务器地址输入框
    m_serverEdit = new QLineEdit(m_serverWidget);

    // 添加按钮
    m_addServerBtn = new QPushButton(m_serverWidget);
    m_addServerBtn->setMinimumWidth(100);
    m_addServerBtn->setMaximumWidth(100);
    m_addServerBtn->setText(QStringLiteral("添加"));

    // 服务器列表
    m_serverListWidget = new QListWidget(m_serverWidget);
    m_serverListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_serverListWidget->setMaximumHeight(120);

    // 右侧按钮布局
    QVBoxLayout *btnLayout = new QVBoxLayout();
    
    // 删除按钮
    m_removeServerBtn = new QPushButton(m_serverWidget);
    m_removeServerBtn->setText(QStringLiteral("删除"));
    m_removeServerBtn->setEnabled(false);  // 默认禁用，选中列表项后启用

    // 公共服务器列表按钮
    m_publicServerBtn = new QPushButton(m_serverWidget);
    m_publicServerBtn->setText(QStringLiteral("服务器列表"));

    btnLayout->addWidget(m_removeServerBtn);
    btnLayout->addWidget(m_publicServerBtn);

    // 添加到网格布局
    serverLayout->addWidget(serverLabel, 0, 0);
    serverLayout->addWidget(m_serverEdit, 1, 0);
    serverLayout->addWidget(m_addServerBtn, 1, 1);
    serverLayout->addWidget(m_serverListWidget, 2, 0);
    serverLayout->addLayout(btnLayout, 2, 1);

    m_mainLayout->addWidget(m_serverWidget);
}

void QtETOneClick::initButtonArea()
{
    m_bottomWidget = new QWidget(this);
    QGridLayout *bottomLayout = new QGridLayout(m_bottomWidget);

    // 我做房主
    m_hostModeCheckBox = new QtETCheckBtn(m_bottomWidget);
    m_hostModeCheckBox->setText(QStringLiteral("我做房主"));
    m_hostModeCheckBox->setBriefTip("开启后本机作为房主，关闭时为房客");

    // 低延迟优先
    m_latencyFirstCheckBox = new QtETCheckBtn(m_bottomWidget);
    m_latencyFirstCheckBox->setText(QStringLiteral("低延迟优先"));
    m_latencyFirstCheckBox->setBriefTip("低延迟选线，可能可以提升联机体验");
    m_latencyFirstCheckBox->setToolTip(tr("如果开启后出现问题，请关闭"));

    bottomLayout->addWidget(m_hostModeCheckBox, 0, 0);
    bottomLayout->addWidget(m_latencyFirstCheckBox, 0, 1);

    m_mainLayout->addWidget(m_bottomWidget);
}

void QtETOneClick::setupConnections()
{
    // 添加服务器按钮
    connect(m_addServerBtn, &QPushButton::clicked, this, &QtETOneClick::onAddServer);
    
    // 输入框回车添加
    connect(m_serverEdit, &QLineEdit::returnPressed, this, &QtETOneClick::onServerEditReturnPressed);
    
    // 删除服务器按钮
    connect(m_removeServerBtn, &QPushButton::clicked, this, &QtETOneClick::onRemoveServer);
    
    // 服务器列表选中变化
    connect(m_serverListWidget, &QListWidget::itemSelectionChanged, 
            this, &QtETOneClick::onServerSelectionChanged);
}

void QtETOneClick::onAddServer()
{
    QString serverAddr = m_serverEdit->text().trimmed();
    if (serverAddr.isEmpty()) {
        return;
    }

    // 检查是否已存在
    for (int i = 0; i < m_serverListWidget->count(); ++i) {
        if (m_serverListWidget->item(i)->text() == serverAddr) {
            m_serverEdit->clear();
            return;
        }
    }

    // 添加到列表
    m_serverListWidget->addItem(serverAddr);
    m_serverEdit->clear();
}

void QtETOneClick::onServerEditReturnPressed()
{
    onAddServer();
}

void QtETOneClick::onRemoveServer()
{
    int currentRow = m_serverListWidget->currentRow();
    if (currentRow >= 0) {
        delete m_serverListWidget->takeItem(currentRow);
        // 如果列表为空，禁用删除按钮
        if (m_serverListWidget->count() == 0) {
            m_removeServerBtn->setEnabled(false);
        }
    }
}

void QtETOneClick::onServerSelectionChanged()
{
    // 有选中项时启用删除按钮，否则禁用
    bool hasSelection = m_serverListWidget->currentRow() >= 0;
    m_removeServerBtn->setEnabled(hasSelection);
}