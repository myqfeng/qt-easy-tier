#ifndef QTETMAIN_H
#define QTETMAIN_H

#include <QMainWindow>
#include <QStyleHints>

namespace Ui {
class QtETMain;
}

class QtETMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtETMain(QWidget *parent = nullptr);
    ~QtETMain();

private:
    Ui::QtETMain *ui;

private slots:
    void onSchemeChanged(const Qt::ColorScheme &scheme); // 系统颜色变化时调用
};

#endif // QTETMAIN_H
