#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPainter>
#include <QDebug>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;

public:

    void paintEvent(QPaintEvent *event);
    void getInput();
    int setPcNum = 7; //子节点的数量

    typedef struct{
        int lineColor;
        QString ipaddr;
    }pcInfo;
    QList<pcInfo> connectLists;

private slots:
    void on_btn_refresh_clicked();
};

#endif // DIALOG_H
