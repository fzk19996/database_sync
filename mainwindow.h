#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>
#include <QDateTime>
#include <QSqlError>
#include <QList>
#include <QTimer>
#include <QFile>
#include "transfer_log.h"
#include "error_log.h"
#include "source_dingyue.h"
#include "source_peizhi.h"
#include "destination_dingyue.h"
#include "condition_monitor.h"
#include "dialog.h"
#include "netWindow.h"

#define DEBUG 1

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString delete_str(QString sql);//处理TEMP表中的删除语句
    QString insert_str(QString sql);//处理TEMP表中的插入语句
    void add_log(int type,int number);//添加log记录
    void add_error_log();//添加error_log记录

    void add_trigger(QString pattern_table);//添加触发器

    void add_sync_database(QString sql);



 signals:

private slots:
    void on_pushButton_clicked();

    //void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_7_clicked();

    void on_pushButtonSYN_clicked();

    void syn_fun();//同步函数，生成需同步的文件

    void get_data();//源端+订阅发布||源端处理SYSDBA.TEMP中要同步的数据,放在SYSDBA.TRANSFER和txt文件中

    void get_data_transfer();//源端+配置发布||源端处理SYSDBA.TEMP中要同步的数据,放在SYSDBA.TRANSFER和txt文件中，并将txt文件发送和删除

    void get_data_from_source();//目的端+订阅发布||目的端和源端进行通信，获得更新后的数据

    void AttachChose();

    void AttachChose_mode();

    void infoRecv_source_dingyue_pattern_table(QString);//接收源端订阅发布传来的模式.表信息

    void infoRecv_source_peizhi_pattern_table(QString);//接收源端配置发布传来的模式.表信息

    void infoRecv_source_peizhi_user_ip_port(QString);//接收源端配置发布传来的用户ip_port信息

    void infoRecv_des_dingyue_ip_port(QString);//接收目的端订阅发布传来的用户ip_port信息

    void on_pushButton_test_clicked();

    void on_pushButton_condition_monitor_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    netWindow* _n_d;
    transfer_log* l;
    error_log* e_l;
    source_dingyue* s_d;
    source_peizhi* s_p;
    destination_dingyue* d_d;
    condition_monitor* c_m;
    Dialog* d_l;
    QSqlDatabase db;
    int id;
    QString err;
    QList<QString> date_type_list_need_quotation = { "CHAR", "CHARACTER", "VARCHAR", "DATE", "TIME", "TIMESTAMP", "TEXT"};
    QString file_path = "/tmp/testqt.txt";
    // QString file_path = "C://Users//think//Desktop//testqt.txt";
    //记录源端订阅发布的“模式.表”内容
    QList<QString> source_dingyue_list = {};
    //记录源端配置发布的用户“ip_port”内容
    QList<QString> source_peizhi_list = {};
    //记录订阅发布目的端选择的源端ip和端口数据，list[0]为ip，list[1]为端口
    QList<QString> source_peizhi_ip_port_list = {};
    //记录订阅发布目的端选择的源端ip和端口数据，list[0]为ip，list[1]为端口
    QList<QString> des_dingyue_source_ip_port_list = {};


};
#endif // MAINWINDOW_H

