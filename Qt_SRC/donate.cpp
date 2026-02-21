#include "donate.h"
#include "ui_donate.h"

Donate::Donate(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Donate)
{
    ui->setupUi(this);
    
    // 连接关闭按钮点击信号到窗口关闭
    connect(ui->pushButton, &QPushButton::clicked, this, &Donate::close);
}

Donate::~Donate()
{
    delete ui;
}
