#ifndef CONDITION_MONITOR_H
#define CONDITION_MONITOR_H

#include <QWidget>
#include <QPainter>
#include <QDebug>

namespace Ui {
class condition_monitor;
}

class condition_monitor : public QWidget
{
    Q_OBJECT

public:
    explicit condition_monitor(QWidget *parent = nullptr);
    ~condition_monitor();

    void paintEvent(QPaintEvent *event);
    void getInput();
    int setPcNum = 7; //子节点的数量

    typedef struct{
        int lineColor;
        QString ipaddr;
    }pcInfo;
    QList<pcInfo> connectLists;

private:
    Ui::condition_monitor *ui;
};

#endif // CONDITION_MONITOR_H
