#include "qtetmain.h"
#include "ui_qtetmain.h"

#include <QPalette>
#include <qstyle.h>
#include <QStyleHints>

QtETMain::QtETMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtETMain)
{
    ui->setupUi(this);

    // 设置一次调色板
    const Qt::ColorScheme &currentScheme = QGuiApplication::styleHints()->colorScheme();
    onSchemeChanged(currentScheme);
    // 监测系统调色板变化
    const QStyleHints *hints = QGuiApplication::styleHints();
    connect(hints, &QStyleHints::colorSchemeChanged, this, &QtETMain::onSchemeChanged);
}

QtETMain::~QtETMain()
{
    delete ui;
}

void QtETMain::onSchemeChanged(const Qt::ColorScheme &scheme)
{
    if (scheme == Qt::ColorScheme::Dark) {

        // 处理暗黑模式，设置sideWidget背景调色板为深色
        ui->sideWidget->setPalette(QPalette(Qt::black));
        ui->sideWidget->setAutoFillBackground(true);
        qApp->setPalette(QPalette(QColor("#222222")));
        // 强制更新一次应用的调色板
        qApp->style()->unpolish(qApp);
        qApp->style()->polish(qApp);
        qApp->processEvents();


    } else {
        // 处理亮色模式，设置sideWidget背景调色板为浅色
        ui->sideWidget->setPalette(QPalette(QColor("#f0f0f0")));
        ui->sideWidget->setAutoFillBackground(true);
        qApp->setPalette(QPalette(Qt::white));
        // 强制更新一次应用的调色板
        qApp->style()->unpolish(qApp);
        qApp->style()->polish(qApp);
        qApp->processEvents();
    }
}