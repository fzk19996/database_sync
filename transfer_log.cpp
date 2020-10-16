#include "transfer_log.h"
#include "ui_transfer_log.h"

transfer_log::transfer_log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::transfer_log)
{
    ui->setupUi(this);
}

transfer_log::~transfer_log()
{
    delete ui;
}

//查询同步任务日志
void transfer_log::on_pushButton_2_clicked()
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("SYSDBA.LOG");
    model->setSort(1, Qt::AscendingOrder);
    model->setHeaderData(0, Qt::Horizontal, "时间");
    model->setHeaderData(1, Qt::Horizontal, "任务描述");
    model->setHeaderData(2, Qt::Horizontal, "更改条数");
    model->select();

    QTableView *view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->resizeColumnsToContents();
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);
    ui->mission_log->setModel(model);
    ui->mission_log->show();
//    view->show();
}
