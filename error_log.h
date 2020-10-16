#ifndef ERROR_LOG_H
#define ERROR_LOG_H

#include <QDialog>
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

namespace Ui {
class error_log;
}

class error_log : public QDialog
{
    Q_OBJECT

public:
    explicit error_log(QWidget *parent = nullptr);
    ~error_log();

private:
    Ui::error_log *ui;
public:
    void showtable();
private slots:
    void on_pushButton_clicked();
};

#endif // ERROR_LOG_H
