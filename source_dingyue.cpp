#include "source_dingyue.h"
#include "ui_source_dingyue.h"

source_dingyue::source_dingyue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::source_dingyue)
{
    ui->setupUi(this);
    //combox_pattern显示内容为所有可用的模式名，根据模式名显示模式内的表名
    QString sql= "select distinct object_name TABLE_SCHEMA from all_objects where object_type = 'SCH'";
    QSqlQuery query;
    query.exec(sql);
    ui->comboBox_pattern->clear();
    while(query.next())
    {
        ui->comboBox_pattern->addItem(query.value(0).toString());
    }
    connect(ui->comboBox_pattern,SIGNAL(currentIndexChanged(int)),this,SLOT(AttachChose()));

}

source_dingyue::~source_dingyue()
{
    delete ui;
}

void source_dingyue::AttachChose()
{
    int currentIndex=ui->comboBox_pattern->currentIndex();
    QString pattern_name = ui->comboBox_pattern->currentText();
    QString sql= "select TABLE_NAME from dba_tables where owner='";
    sql+=pattern_name;
    sql+="'";
    QSqlQuery query;
    //查询当前模式下的表名
    query.exec(sql);
    switch (currentIndex) {
    default:
        ui->comboBox_table->clear();
        while (query.next()) {
            ui->comboBox_table->addItem(query.value(0).toString());
        }
        break;
    }
}

//添加按钮，点击，添加对应模式和表至发布数据中
void source_dingyue::on_pushButton_add_clicked()
{
    QString pattern,table,p_t;
    pattern = ui->comboBox_pattern->currentText();
    table = ui->comboBox_table->currentText();
    p_t = pattern+"."+table;
    emit infoSend(p_t);
}

//完成按钮，发出信号给mainwindow，添加订阅发布信息
void source_dingyue::on_pushButton_over_clicked()
{
    QString send_message="over";
    emit infoSend(send_message);
    this->close();
}
