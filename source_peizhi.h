#ifndef SOURCE_PEIZHI_H
#define SOURCE_PEIZHI_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>
#include <QRegExpValidator>


namespace Ui {
class source_peizhi;
}

class source_peizhi : public QDialog
{
    Q_OBJECT

public:
    explicit source_peizhi(QWidget *parent = nullptr);
    ~source_peizhi();

private:
    Ui::source_peizhi *ui;

private slots:
    void AttachChose();

    void on_pushButton_add_data_clicked();

    void on_pushButton_add_user_clicked();

    void on_pushButton_over_clicked();

signals:
    void infoSend(QString);
    void infoSend_user(QString);
};

#endif // SOURCE_PEIZHI_H
