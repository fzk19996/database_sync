#ifndef TRANSFER_LOG_H
#define TRANSFER_LOG_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>
#include <QDateTime>
#include <QSqlError>
#include <QStandardItemModel>
#include <QDialog>

namespace Ui {
class transfer_log;
}

class transfer_log : public QDialog
{
    Q_OBJECT

public:
    explicit transfer_log(QWidget *parent = nullptr);
    ~transfer_log();

private:
    Ui::transfer_log *ui;

public:
    void showtable();
private slots:
    void on_pushButton_2_clicked();
};
#endif // TRANSFER_LOG_H
