#include "error_log.h"
#include "ui_error_log.h"

error_log::error_log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::error_log)
{
    ui->setupUi(this);
}

error_log::~error_log()
{
    delete ui;
}

void error_log::on_pushButton_clicked()
{
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("SYSDBA.ERROR_LOG");
    model->setSort(1, Qt::AscendingOrder);
    model->setHeaderData(0, Qt::Horizontal, "时间");
    model->setHeaderData(1, Qt::Horizontal, "错误描述");
    model->select();

    QTableView *view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->resizeColumnsToContents();
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *header = view->horizontalHeader();
    header->setStretchLastSection(true);
    ui->tableview->setModel(model);
    ui->tableview->show();
}
