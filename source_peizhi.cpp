#include "source_peizhi.h"
#include "ui_source_peizhi.h"

source_peizhi::source_peizhi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::source_peizhi)
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

    QRegExp regExpIP("(((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])|localhost)");
    QRegExp regExpNetPort("((6553[0-5])|[655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[0-9])");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(regExpIP,this));
    ui->lineEdit_port->setValidator(new QRegExpValidator(regExpNetPort,this));
}

source_peizhi::~source_peizhi()
{
    delete ui;
}

void source_peizhi::AttachChose()
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
void source_peizhi::on_pushButton_add_data_clicked()
{
    QString pattern,table,p_t;
    pattern = ui->comboBox_pattern->currentText();
    table = ui->comboBox_table->currentText();
    p_t = pattern+"."+table;
    emit infoSend(p_t);
}

void source_peizhi::on_pushButton_add_user_clicked()
{
    QString ip,port,i_p;
    ip = ui->lineEdit_ip->text();
    port = ui->lineEdit_port->text();
    i_p = ip+"_"+port;
    emit infoSend_user(i_p);
}

void source_peizhi::on_pushButton_over_clicked()
{
    QString send_message="over";
    emit infoSend(send_message);
    this->close();
}
