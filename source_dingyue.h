#ifndef SOURCE_DINGYUE_H
#define SOURCE_DINGYUE_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>

namespace Ui {
class source_dingyue;
}

class source_dingyue : public QDialog
{
    Q_OBJECT

public:
    explicit source_dingyue(QWidget *parent = nullptr);
    ~source_dingyue();

private:
    Ui::source_dingyue *ui;

private slots:
    void AttachChose();
    void on_pushButton_add_clicked();

    void on_pushButton_over_clicked();

signals:
    void infoSend(QString);
};




#endif // SOURCE_DINGYUE_H
