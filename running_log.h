#ifndef RUNNING_LOG_H
#define RUNNING_LOG_H

#include <QDialog>

namespace Ui {
class running_log;
}

class running_log : public QDialog
{
    Q_OBJECT

public:
    explicit running_log(QWidget *parent = nullptr);
    ~running_log();

private:
    Ui::running_log *ui;
};

#endif // RUNNING_LOG_H
