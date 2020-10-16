#include "destination_dingyue.h"
#include "ui_destination_dingyue.h"

destination_dingyue::destination_dingyue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::destination_dingyue)
{
    ui->setupUi(this);
    QRegExp regExpIP("(((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])|localhost)");
    QRegExp regExpNetPort("((6553[0-5])|[655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[0-9])");
    ui->lineEdit->setValidator(new QRegExpValidator(regExpIP,this));
    ui->lineEdit_2->setValidator(new QRegExpValidator(regExpNetPort,this));
}

destination_dingyue::~destination_dingyue()
{
    delete ui;
}

void destination_dingyue::on_pushButton_over_clicked()
{
    QString ip,port,i_p;
    ip = ui->lineEdit->text();
    port = ui->lineEdit_2->text();
    i_p = ip+"_"+port;
    emit infoSend_user(i_p);
    this->close();
}
