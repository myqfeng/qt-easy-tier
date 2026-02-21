#ifndef DONATE_H
#define DONATE_H

#include <QDialog>

namespace Ui {
class Donate;
}

class Donate : public QDialog
{
    Q_OBJECT

public:
    explicit Donate(QWidget *parent = nullptr);
    ~Donate();

private:
    Ui::Donate *ui;
};

#endif // DONATE_H
