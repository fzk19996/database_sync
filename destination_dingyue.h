#ifndef DESTINATION_DINGYUE_H
#define DESTINATION_DINGYUE_H

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
class destination_dingyue;
}

class destination_dingyue : public QDialog
{
    Q_OBJECT

public:
    explicit destination_dingyue(QWidget *parent = nullptr);
    ~destination_dingyue();

private slots:
    void on_pushButton_over_clicked();

private:
    Ui::destination_dingyue *ui;

signals:
    void infoSend_user(QString);
};

#endif // DESTINATION_DINGYUE_H
