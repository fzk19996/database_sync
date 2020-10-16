#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transfer_log.h"
#include "error_log.h"
#include "condition_monitor.h"
#include <QDateTime>
#include <QStandardItemModel>
#include <QFileDialog>
#include <iostream>

#define DINGYUE_MISSION_CREATE 0
#define DINGYUE_MISSION_UPDATE_DATA 1
#define PEIZHI_MISSION_CREATE 2
#define PEIZHI_MISSION_FABU 3

#pragma execution_character_set("utf-8")

/**使用到的数据库表：
 * 1.SYSDBA.TEMP表：只有一个字段。存放原始的，记录数据变更对应操作的sql语句，由触发器生成，但是该表中的sql语句缺少对字符串类型数值的引号，例如"NAME=xiaoming"，在结尾会有"$type1|type2|···|"标识对应字段的数值类型。
 * 2.SYSDBA.TRANSFER表：只有一个字段。对SYSDBA.TEMP表中的sql语句进行处理，需要增加引号的字段增加引号，形成可执行的sql语句，并会将“改”语句拆分为两条sql语句，存至该表中，并清空TEMP表，该表的内容为需要发送的内容。
 * 3.SYSDBA.LOG表：3个字段。记录同步任务的运行情况，包括“时间”，“任务描述”，“更改条数”。
 * 4.SYSDBA.ERROR_LOG表：2个字段。记录程序运行中的错误情况，包括“时间”，“错误描述”
 */

/*数据修改注意：
 * 增删改数据后一定要执行“COMMIT”语句，数据才会真正写到数据库中，否则可能读不到
 */

/**订阅发布的四种情况：
 * 1.源端+订阅发布：添加触发器，记录新增数据
 * 2.源端+配置发布：添加触发器，记录新增数据，主动发布给对应用户
 * 3.目的端+订阅发布：向源端发布请求数据同步，监听数据输入
 * 4.目的端+配置发布：不需要操作（监听数据输入）
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    _n_d = new netWindow(this);
    ui->setupUi(this);
    QObject::connect(this->ui->pushButton,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked));
    QObject::connect(this->ui->pushButton_3,SIGNAL(clicked()),this,SLOT(on_pushButton_3_clicked));

    //输入框内容验证
    QRegExp regExpIP("(((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])|localhost)");
    QRegExp regExpNetPort("((6553[0-5])|[655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[0-9])");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(regExpIP,this));
//    ui->lineEdit_ip_op->setValidator(new QRegExpValidator(regExpIP,this));
    ui->lineEdit_port->setValidator(new QRegExpValidator(regExpNetPort,this));
//    ui->lineEdit_port_op->setValidator(new QRegExpValidator(regExpNetPort,this));

    //将同步策略与同步频率相连接，实现：
    /*    |订阅发布|配置发布
     * --------------------
     * 源端|更新频率|同步频率
     * --------------------
     * 目的|同步频率|同步频率(持续监听)
     */
    connect(ui->comboBox_character,SIGNAL(currentIndexChanged(int)),this,SLOT(AttachChose()));

    connect(ui->comboBox_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(AttachChose_mode()));

}

void MainWindow::AttachChose()
{
    int currentIndex=ui->comboBox_character->currentIndex();
    switch (currentIndex) {
    //如果选择是源端
    case 0:
        if(ui->comboBox_mode->currentIndex() == 0){
            ui->label_8->setText("数据更新频率");
            ui->comboBox_freq->clear();
            ui->comboBox_freq->addItem("1秒");
            ui->comboBox_freq->addItem("5秒");
            ui->comboBox_freq->addItem("30秒");
        }
        else {
            ui->label_8->setText("同步频率");
            ui->comboBox_freq->clear();
            ui->comboBox_freq->addItem("仅1次");
            ui->comboBox_freq->addItem("5分钟");
            ui->comboBox_freq->addItem("10分钟");
        }
        break;
    //如果选择是目的端
    case 1:
        ui->label_8->setText("同步频率");
        ui->comboBox_freq->clear();
        if(ui->comboBox_mode->currentIndex() == 0){
            ui->comboBox_freq->addItem("仅1次");
            ui->comboBox_freq->addItem("5分钟");
            ui->comboBox_freq->addItem("10分钟");
        }
        else {
            ui->comboBox_freq->addItem("持续监听");
        }
        break;
    default:
        break;
    }
}

void MainWindow::AttachChose_mode()
{
    int currentIndex=ui->comboBox_mode->currentIndex();
    switch (currentIndex) {
    //如果选择是订阅发布
    case 0:
        if(ui->comboBox_character->currentIndex() == 0){
            ui->label_8->setText("数据更新频率");
            ui->comboBox_freq->clear();
            ui->comboBox_freq->addItem("1秒");
            ui->comboBox_freq->addItem("5秒");
            ui->comboBox_freq->addItem("30秒");
        }
        else {
            ui->label_8->setText("同步频率");
            ui->comboBox_freq->clear();
            ui->comboBox_freq->addItem("仅1次");
            ui->comboBox_freq->addItem("5分钟");
            ui->comboBox_freq->addItem("10分钟");
        }
        break;
    //如果选择是配置发布
    case 1:
        ui->label_8->setText("同步频率");
        ui->comboBox_freq->clear();
        if(ui->comboBox_character->currentIndex() == 0){
            ui->comboBox_freq->addItem("仅1次");
            ui->comboBox_freq->addItem("5分钟");
            ui->comboBox_freq->addItem("10分钟");
        }
        else {
            ui->comboBox_freq->addItem("持续监听");
        }
        break;
    default:
        break;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


//添加触发器
void MainWindow::add_trigger(QString pattern_table)
{
    qDebug()<<"add trigger"<<"\n"<<pattern_table;
    QString sql,insert_str,get_column_name_str,type_str;
    QStringList strarray=pattern_table.split(".");
#ifdef DEBUG
    sql = "CREATE TABLE SYSDBA.TEMP(SQL_STR VARCHAR(128));";
#else
    QString pattern = strarray[0];//模式名
    QString table = strarray[1];//表名
    QSqlQuery m_query;
    QStringList des_array = ui->lineEdit_map->text().split(".");
    bool ins = false, del = false, upd = false;
    // 创建数据暂存表TEMP
    sql = "CREATE TABLE SYSDBA.TEMP(SQL_STR VARCHAR(128));";
    m_query.exec(sql);

    //切换模式到要创建触发器的模式中
    sql = "SET SCHEMA "+pattern+";";
    if(!m_query.exec(sql)){
        QMessageBox::about(nullptr,"错误:","无法访问该模式！");
        return;
    }

    /*
    *创建增触发器
    */

    sql = "CREATE OR REPLACE TRIGGER TRG_INS_"+pattern+"_"+table+"\n";
    sql += "BEFORE INSERT ON "+pattern+"."+table+"\n";
    sql += "FOR EACH ROW\n";
    sql += "DECLARE";
    sql += "  STR CHAR(128);\n";
    sql += "BEGIN\n";
    //获得当前表的列名及列的数据类型
    get_column_name_str = "select COLUMN_NAME,DATA_TYPE from user_tab_columns where Table_Name='"+table+"';";
    m_query.exec(get_column_name_str);
    //生成存入数据库的语句
    sql += "STR:='INSERT INTO " +des_array[0]+"."+des_array[1] + " VALUES('";

    type_str = "$";
    while(m_query.next())
    {
        sql += "||:NEW." + m_query.value(0).toString()+"||"+"','";
        type_str += m_query.value(1).toString()+'|';
    }
    sql = sql.left(sql.length() - 5);
    sql += "||');"+type_str+"';\n";
    sql += "INSERT INTO SYSDBA.TEMP VALUES(STR);\n";

    sql += "END;";
    if(!m_query.exec(sql)){
        QMessageBox::about(nullptr,"错误","添加增触发器失败！");
        return;
    }
    else{
        ins = true;
    }

    /*
    *创建删触发器
    */
    sql = "CREATE OR REPLACE TRIGGER TRG_DEL_"+pattern+"_"+table+"\n";
    sql += "AFTER DELETE ON "+pattern+"."+table+"\n";
    sql += "FOR EACH ROW\n";
    sql += "DECLARE";
    sql += "  STR CHAR(128);\n";
    sql += "BEGIN\n";
    //获得当前表的列名及列的数据类型
    get_column_name_str = "select COLUMN_NAME,DATA_TYPE from user_tab_columns where Table_Name='"+table+"';";
    m_query.exec(get_column_name_str);
    //生成存入数据库的语句
    sql += "STR:='DELETE FROM " +des_array[0]+"."+des_array[1] + " WHERE '";

    type_str = "$";
    while(m_query.next())
    {
        //||'T1='||:OLD.NUM||'AND '
        sql += "||'"+m_query.value(0).toString()+"='"+"||:OLD." + m_query.value(0).toString()+"||"+"' AND '";
        type_str += m_query.value(1).toString()+'|';
    }
    sql = sql.left(sql.length() - 9);
    sql += "||';" + type_str + "';\n";
    sql += "INSERT INTO SYSDBA.TEMP VALUES(STR);\n";

    sql += "END;";
    qDebug()<<sql;
    if(!m_query.exec(sql)){
        QMessageBox::about(nullptr,"错误","添加删触发器失败！");
        return;
    }
    else{
        del = true;
    }

    /*
    *创建改触发器
    *改的操作分解为先删除后插入
    */
    sql = "CREATE OR REPLACE TRIGGER TRG_UPD_"+pattern+"_"+table+"\n";
    sql += "BEFORE UPDATE ON "+pattern+"."+table+"\n";
    sql += "FOR EACH ROW\n";
    sql += "DECLARE";


    sql += "  STR CHAR(256);\n";
    sql += "BEGIN\n";
    //获得当前表的列名及列的数据类型
    get_column_name_str = "select COLUMN_NAME,DATA_TYPE from user_tab_columns where Table_Name='"+table+"';";
    m_query.exec(get_column_name_str);
    //生成存入数据库的语句
    //这里改成两个合在一起的语句，先删后写
    sql += "STR:='DELETE FROM " +des_array[0]+"."+des_array[1] + " WHERE '";

    type_str = "$";
    while(m_query.next())
    {
        sql += "||'"+m_query.value(0).toString()+"='"+"||:OLD." + m_query.value(0).toString()+"||"+"' AND '";
        type_str += m_query.value(1).toString()+'|';
    }
    sql = sql.left(sql.length() - 9);
    sql += "||';" + type_str + "->'";//使用->区分本条语句和下一条语句


    sql += "||'INSERT INTO " +des_array[0]+"."+des_array[1] + " VALUES('";

    m_query.exec(get_column_name_str);
    while(m_query.next())
    {
        sql += "||:NEW." + m_query.value(0).toString()+"||"+"','";
    }
    sql = sql.left(sql.length() - 5);
    sql += "||');"+type_str+"';\n";

    sql += "INSERT INTO SYSDBA.TEMP VALUES(STR);\n";

    sql += "END;";
    qDebug()<<sql;
    if(!m_query.exec(sql)){
        QMessageBox::about(nullptr,"错误","添加改触发器失败！");
        return;
    }
    else{
        upd = true;
    }
    if(ins && del && upd){
        QMessageBox::about(nullptr,"成功","添加数据表成功！");
    }
#endif
    _n_d->_data_send = sql;
    _n_d->show();

}

//连接本地数据库
void MainWindow::on_pushButton_7_clicked()
{
    db=QSqlDatabase::addDatabase("QODBC");
    db.setPort(ui->lineEdit_port->text().toInt());
    db.setHostName(ui->lineEdit_ip->text());
    db.setDatabaseName(ui->lineEdit_dbname->text());
    db.setUserName(ui->lineEdit_username->text());
    db.setPassword(ui->lineEdit_password->text());
    // ODBC DSN信息存储在/etc/odbc.ini中
    QFile file("/etc/odbc.ini");

    QString line;
    QString dbname,server,uid,pwd,port;
    bool record = false,find_dsn = false;
    bool setting_localhost = false,input_localhost = false;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            line = file.readLine();

            if(line.indexOf('[') != -1){
                dbname = line.section('[',1,1);
                dbname = dbname.section(']',0,0);
                if(dbname == ui->lineEdit_dbname->text()){
                   record = true;
                }
                else {
                    record = false;
                }
            }

            if(line.indexOf("SERVER") != -1 && record)
            {
                server = line.section('=',1,1);
                server = server.section('\n',0,0);
                server.remove(QRegExp("\\s"));
            }

            if(line.indexOf("UID") != -1 && record)
            {
                uid = line.section('=',1,1);
                uid = uid.section('\n',0,0);
                uid.remove(QRegExp("\\s"));
            }
            if(line.indexOf("PWD") != -1 && record)
            {
                pwd = line.section('=',1,1);
                pwd = pwd.section('\n',0,0);
                pwd.remove(QRegExp("\\s"));
            }
            if(line.indexOf("TCP_PORT") != -1 && record)
            {
                port = line.section('=',1,1);
                port = port.section('\n',0,0);
                port.remove(QRegExp("\\s"));
            }

            if(server == "127.0.0.1" || server == "localhost"){
                setting_localhost = true;
            }
            if(ui->lineEdit_ip->text() == "127.0.0.1" || ui->lineEdit_ip->text() == "localhost"){
                input_localhost = true;
            }

            if(record && (server == ui->lineEdit_ip->text() || (setting_localhost && input_localhost)) && uid == ui->lineEdit_username->text() && pwd == ui->lineEdit_password->text() && port == ui->lineEdit_port->text()){
                qDebug() << "find dsn";
                find_dsn = true;
                break;
            }
        }
        if(!find_dsn){
            QMessageBox::about(nullptr,"错误:","连接数据库失败!请检查输入是否正确！");
            return;
        }
        file.close();
    }

    if(!db.open()){
        QMessageBox::about(nullptr,"错误:","连接数据库失败!错误代码:\n"+db.lastError().text());
        qDebug()<< db.lastError().text();
        return;
    }
    else{
        QMessageBox::about(nullptr,"提示:","连接数据库成功!");
    }
}


//处理删除语句，对字符串类型的列加引号
QString MainWindow::delete_str(QString sql)
{
    //e.g.DELETE FROM Model2.Table2 WHERE NUM=1 AND NAME=NAME;$INT|VARCHAR|
    //去掉结尾多余的‘|’
    sql = sql.left(sql.length() - 1);
    QString dele = sql.split('$')[0];
    QStringList colume_type = sql.split('$')[1].split('|');
    int begin_index = 0,result_index;
    for (int index=0;index<colume_type.length();index++) {
        //找等号，等号后面的字符串是有可能需要处理的
        result_index = sql.indexOf('=',begin_index)+1;
        begin_index = result_index;
        //如果类型属于需要加引号的类型char，则写sql语句时需要补充上单引号
        if(date_type_list_need_quotation.contains(colume_type[index])){
            for (;sql[begin_index] != ' ' && sql[begin_index] != ';';begin_index++) {
                //找到需要加引号的字段，例如“NAME”
            }
            QString child_str = sql.mid(result_index,begin_index-result_index);
            child_str = "''"+child_str+"''";
            sql.replace(result_index,begin_index-result_index,child_str);
        }
    }
    sql = sql.split('$')[0];
    qDebug()<<sql;
    return sql;
}

//处理插入语句，对字符串类型的列加引号
QString MainWindow::insert_str(QString sql)
{
    //e.g.INSERT INTO Model2.Table2 VALUES(1,NAME);$INT|VARCHAR|
    sql = sql.trimmed();
    sql = sql.left(sql.length() - 1);

    QString dele = sql.split('$')[0];
    QStringList colume_type = sql.split('$')[1].split('|');
    int begin_index,end_index;
    begin_index = sql.indexOf('(',0);
    end_index = sql.indexOf(')',0);
    //生成values_list,长度和column_type一致
    QStringList values_list= sql.mid(begin_index+1,end_index-begin_index-1).split(',');
    QString values_str="";
    for (int index=0;index<colume_type.length();index++) {
        //如果类型是char，则写sql语句时需要补充上单引号
        if(date_type_list_need_quotation.contains(colume_type[index])){
            values_list[index] = "''"+values_list[index]+"''";
        }
    }
    for (int index=0;index<values_list.length();index++) {
        values_str += values_list[index]+",";
    }
    values_str[values_str.length()-1] = ')';

    sql = sql.split('(')[0]+"("+values_str+";";
    qDebug()<<sql;
    return sql;
}

//添加log记录，参数包含：任务状态和新增数据
//LOG字段添加身份信息：源端/目的端
//LOG表包含的字段为：时间，任务名称，任务描述，同步策略，用户身份，任务状态，新增数据
void MainWindow::add_log(int type,int number)
{
    QSqlQuery query;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd"),mode;
    switch (type) {
    case DINGYUE_MISSION_CREATE:
        mode = "订阅发布任务创建";
        break;
    case DINGYUE_MISSION_UPDATE_DATA:
        mode = "订阅发布任务数据更新";        
        break;
    case PEIZHI_MISSION_CREATE:
        mode = "配置发布任务创建";        
        break;
    case PEIZHI_MISSION_FABU:
        mode = "配置任务发布";        
        break;
    }
    //添加LOG记录--首次添加，内容为记录同步任务被创建
    QString sql_str = "INSERT INTO SYSDBA.LOG VALUES('"+current_date+"','"+ui->lineEdit_mission_name->text()+"','"+ui->lineEdit_mission_dsp->text()+"','"+ui->comboBox_mode->currentText()+"','"+ui->comboBox_character->currentText()+"','"+mode+"',"+QString::number(number)+");";
    qDebug()<<sql_str;
    bool v = query.exec(sql_str);
    qDebug()<<mode<<"add_log:"<<v;
    query.exec("COMMIT");
}

//处理SYSDBA.TEMP中要同步的数据,放在SYSDBA.TRANSFER中，并从SYSDBA.TRANSFER中读出放在txt文件中
void MainWindow::get_data()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug()<<"源端+订阅发布：get_data函数启动 "<<current_date;
    QSqlQuery query1;
    QString sql_str,write_sql_str;
    //防止数据为空
    query1.exec("COMMIT;");
    query1.exec("SELECT * FROM SYSDBA.TEMP;");
    while (query1.next()) {
        sql_str = query1.value(0).toString();
        QString s = sql_str.trimmed();
        qDebug()<<s;
        //如果含有“->”，则说明有两条语句
        if(sql_str.contains("->")){
            QStringList sen_list = sql_str.split("->");
            write_sql_str = delete_str(sen_list[0]);
            write_sql_str += insert_str(sen_list[1]);
        }
        else if (sql_str.contains("INSERT")) {
            //处理插入语句
            write_sql_str = insert_str(sql_str);
        }
        else {
            //处理删除语句
            write_sql_str = delete_str(sql_str);
        }
        QSqlQuery query;
        QString sql_save2transfer = "INSERT INTO SYSDBA.TRANSFER VALUES('"+write_sql_str+"');";
        query.exec(sql_save2transfer);
    }
    //添加log记录
    query1.exec("SELECT count(*) FROM SYSDBA.TEMP");
    query1.next();
    int change_num = query1.value(0).toInt();
    qDebug()<<"change num:"<<change_num;

    //如果有数据更新则添加记录
    if(change_num !=0){
        add_log(DINGYUE_MISSION_UPDATE_DATA,change_num);
    }
    //处理完TEMP数据库的数据后，数据已被转移到transfer数据库等待发送，这时清空TEMP数据库,等待下一时段的数据
    query1.exec("DELETE SYSDBA.TEMP;");
    query1.exec("SELECT * FROM SYSDBA.TRANSFER;");
    //windows测试中将文件存储在"C://Users//think//Desktop//testqt.txt"中
    //linux测试中将文件存储在"/home/testqt.txt"中

    QFile file(file_path);
    //如果txt文件存在，代表还没有发送，需要在原始数据基础进行添加
    if (file.exists()){
        qDebug()<<"file exist";
        file.open( QIODevice::Append | QIODevice::Text );
    }
    //如果txt文件不存在，代表是第一次写，需要创建一个新文件进行读写
    else {
        file.open( QIODevice::ReadWrite | QIODevice::Text );
    }
    QTextStream in(&file);
    while (query1.next()) {
        sql_str = query1.value(0).toString();
        in<<sql_str<<"\n";
    }
    file.close();
    query1.exec("DELETE SYSDBA.TRANSFER;");
}

//处理SYSDBA.TEMP中要同步的数据,放在SYSDBA.TRANSFER中，并从SYSDBA.TRANSFER中读出放在txt文件中，并将数据传输和txt文件删除
void MainWindow::get_data_transfer()
{
    QSqlQuery query1;
    QString sql_str,write_sql_str;
    //防止数据为空
    query1.exec("COMMIT");
    query1.exec("SELECT * FROM SYSDBA.TEMP;");
    while (query1.next()) {
        sql_str = query1.value(0).toString();
        //如果含有“->”，则说明有两条语句
        QString s = sql_str.trimmed();
        qDebug()<<s;
        if(sql_str.contains("->")){

            QStringList sen_list = sql_str.split("->");
            write_sql_str = delete_str(sen_list[0]);
            write_sql_str += insert_str(sen_list[1]);
        }
        else if (sql_str.contains("INSERT")) {
            //处理插入语句
            write_sql_str = insert_str(sql_str);
        }
        else {
            //处理删除语句
            write_sql_str = delete_str(sql_str);
        }
        QSqlQuery query;
        QString sql_save2transfer = "INSERT INTO SYSDBA.TRANSFER VALUES('"+write_sql_str+"');";
        query.exec(sql_save2transfer);
    }
    //添加log记录
    query1.exec("SELECT count(*) FROM SYSDBA.TEMP");
    query1.next();
    int change_num = query1.value(0).toInt();
    qDebug()<<"change num:"<<change_num;
    add_log(PEIZHI_MISSION_FABU,change_num);
    //处理完TEMP数据库的数据后，数据已被转移到transfer数据库等待发送，这时清空TEMP数据库,等待下一时段的数据
    query1.exec("DELETE SYSDBA.TEMP;");
    query1.exec("SELECT * FROM SYSDBA.TRANSFER;");
    //windows测试中将文件存储在"C://Users//think//Desktop//testqt.txt"中
    //linux测试中将文件存储在"/home/testqt.txt"中

    QFile file(file_path);

    //每次都新将一个文件进行读写
    file.open( QIODevice::ReadWrite | QIODevice::Text );
    QTextStream in(&file);
    while (query1.next()) {
        sql_str = query1.value(0).toString();
        in<<sql_str<<"\n";
    }
    file.close();
    query1.exec("DELETE SYSDBA.TRANSFER;");
    /*
     *数据传输函数
     */
}

//目的端与源端进行通信，获得更新后的数据
void MainWindow::get_data_from_source(){
    /*
     *与源端通信，获得文件，测试时直接读取本地文件
     */
    //执行文件内sql语句
    qDebug()<<"get_data_from_source";
    int update_number = 0;
    QFile file(file_path);
    file.open( QIODevice::ReadWrite | QIODevice::Text );
    QSqlQuery query;

    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);
        qDebug()<< str;
        update_number += 1;
    }
    query.exec("COMMIT");
    add_log(DINGYUE_MISSION_UPDATE_DATA,update_number);
    //执行后删除原文件
    file.close();
    bool v = QFile::remove(file_path);
    qDebug()<<v;
}

//同步函数，根据同步身份和同步策略采取不同操作频率决定间隔多久进行一次操作
//文件可能是以自身作为源节点的待同步数据，也可能是以自身作为子节点的同步请求
void MainWindow::syn_fun()
{
    //windows测试中将待传输文件存储在"C://Users//think//Desktop//testqt.txt"中
    //linux测试中将待传输文件存储在"/home/testqt.txt"中

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug()<<current_date;

    //判断用户身份和同步策略,采取不同的操作
    /*
     *源端+订阅发布
     */
    if(ui->comboBox_character->currentText()=="源端" && ui->comboBox_mode->currentText()=="订阅发布"){
        add_log(DINGYUE_MISSION_CREATE,0);
        //如果是源端进行订阅发布，则源端根据选择的数据更新频率检测TEMP表的内容，将其更新至TRANSFER表，并将要传输的内容传输到txt文件中，用append模式，等待目的端读取数据
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(get_data()));
        QString syn_freq= ui->comboBox_freq->currentText();

        if(QString::compare(syn_freq,"1秒") == 0){
            //每隔1秒调用处理函数
            timer->start(1*1000);
        }
        else if(QString::compare(syn_freq,"5秒") == 0){
            //每隔5秒调用处理函数
            timer->start(5*1000);
        }
        else if(QString::compare(syn_freq,"30秒") == 0){
            //每隔30秒调用处理函数
            timer->start(30*1000);
        }
    }
    /*
     *源端+配置发布
     */
    else if(ui->comboBox_character->currentText()=="源端" && ui->comboBox_mode->currentText()=="配置发布"){
        add_log(PEIZHI_MISSION_CREATE,0);
        //如果是源端进行配置发布，则源端根据选择的同步频率，每固定时间后调用get_data_transfer函数，将数据更新至txt文件中，然后调用传输函数进行数据发布
        QString syn_freq= ui->comboBox_freq->currentText();

        //判断同步频率
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(get_data_transfer()));
        if(QString::compare(syn_freq,"仅1次") == 0){
            //直接调用处理函数
            qDebug()<<"仅1次";
            get_data_transfer();
        }
        else if(QString::compare(syn_freq,"5分钟") == 0){
            //每隔5分钟调用处理函数
            timer->start(5*60*1000);
        }
        else if(QString::compare(syn_freq,"10分钟") == 0){
            //每隔10分钟调用处理函数
            timer->start(10*60*1000);
        }
    }
    /*
     *目的端+订阅发布
     */
    else if(ui->comboBox_character->currentText()=="目的端" && ui->comboBox_mode->currentText()=="订阅发布"){
        add_log(DINGYUE_MISSION_CREATE,0);
        QString syn_freq= ui->comboBox_freq->currentText();

        //判断同步频率
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(get_data_from_source()));
        if(QString::compare(syn_freq,"仅1次") == 0){
            //直接调用处理函数
            qDebug()<<"仅1次";
            get_data_from_source();
        }
        else if(QString::compare(syn_freq,"5分钟") == 0){
            //每隔5分钟调用处理函数
            timer->start(5*60*1000);
        }
        else if(QString::compare(syn_freq,"10分钟") == 0){
            //每隔10分钟调用处理函数
            timer->start(10*60*1000);
        }
    }
    /*
     *目的端+配置发布
     */
    else{
        //ui->comboBox_character->currentText()=="目的端" && ui->comboBox_mode->currentText()=="配置发布"
        add_log(PEIZHI_MISSION_CREATE,0);
        /*
         *监听源端传来的数据，获取更新数据，在本地执行
         * 这里应该是监听后调用一个函数，函数内容为读取数据并执行
         */
        int update_number = 0;
        QFile file(file_path);
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        QSqlQuery query;

        while(!file.atEnd()) {
            QByteArray line = file.readLine();
            QString str(line);
            qDebug()<< str;
            update_number += 1;
        }
        query.exec("COMMIT");
        add_log(PEIZHI_MISSION_FABU,update_number);
        //执行后删除原文件
        file.close();
        bool v = QFile::remove(file_path);
        qDebug()<<v;
    }

}

void MainWindow::add_sync_database(QString sql){
    qDebug() << "添加数据库sql" << sql;
    //执行sql
}

//数据同步按钮
//创建LOG TRANSFER ERROR_LOG表；根据“同步时机”决定何时调用同步函数
void MainWindow::on_pushButtonSYN_clicked()
{
    if(!db.open()){
        QMessageBox::about(nullptr,"错误:","你没有连接数据库!");
        return;
    }
    QSqlQuery query1;
    QString sql_str;
    QString syn_time= ui->comboBox_time->currentText();

    /*******同步任务记录表创建-start*********/
    //查找SYSDBA.LOG/TRANSFER/ERROR_LOG表是否建立
    //LOG表包含的字段为：时间，任务名称，任务描述，同步策略，用户身份，任务状态，新增数据
    //TRANSFER表包含的字段为：SQL_STR(进行传输的sql语句)
    //ERROR_LOG表包含的字段为：DT(时间)，DES（错误描述）

    query1.exec("select TABLE_NAME from dba_tables where owner='SYSDBA'");
    bool EXIST_LOG = false,EXIST_TRANSFER = false,EXIST_ERROR_LOG=false;
    while (query1.next()) {
        QString table_name = query1.value(0).toString();
        if(QString::compare(table_name,"LOG") == 0){
            EXIST_LOG = true;
        }
        if(QString::compare(table_name,"TRANSFER") == 0){
            EXIST_TRANSFER = true;
        }
        if(QString::compare(table_name,"ERROR_LOG") == 0){
            EXIST_ERROR_LOG = true;
        }
    }

    //不存在则新建
    if(!EXIST_LOG){
        query1.exec("CREATE TABLE SYSDBA.LOG(\"时间\" VARCHAR(30),\"任务名称\" VARCHAR(128),\"任务描述\" VARCHAR(256),\"同步策略\" VARCHAR(20),\"用户身份\" VARCHAR(20),\"任务状态\" VARCHAR(40),\"新增数据\" INT);");
    }
    if(!EXIST_TRANSFER){
        query1.exec("CREATE TABLE SYSDBA.TRANSFER(SQL_STR VARCHAR(256));");
    }
    if(!EXIST_ERROR_LOG){
        query1.exec("CREATE TABLE SYSDBA.ERROR_LOG(DT VARCHAR(30),DES VARCHAR(256));");
    }
    /*********同步任务记录表创建-end********/

    //判断同步时机
    qDebug()<<syn_time;
    if(QString::compare(syn_time,"立即同步") == 0){
        //直接调用同步函数
        syn_fun();
    }
    else if(QString::compare(syn_time,"5分钟后同步") == 0){
        //5分钟后调用同步函数
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        qDebug()<<current_date;
        QTimer::singleShot(5*1000, this, SLOT(syn_fun()));
    }
    else if(QString::compare(syn_time,"10分钟后同步") == 0){
        //10分钟后调用同步函数
        QTimer::singleShot(10*1000, this, SLOT(syn_fun()));
    }

}

//用来接收 源端订阅发布 添加的模式.表数据，创建触发器
//会接收over信息，如果收到则认为配置完成
void MainWindow::infoRecv_source_dingyue_pattern_table(QString Info)
{
    //将窗口传入的模式.表名存入
    if(Info != "over"){
        source_dingyue_list.append(Info);
        add_trigger(Info);
    }
    else {
        QMessageBox::about(nullptr,"成功","同步信息配置成功！");
    }
    qDebug()<<Info;
}

//用来接收 源端配置发布 添加的模式.表数据，创建触发器
//会接收over信息，如果收到则认为配置完成
void MainWindow::infoRecv_source_peizhi_pattern_table(QString Info)
{
    //将窗口传入的模式.表名存入
    if(Info != "over"){
        source_peizhi_list.append(Info);
        add_trigger(Info);
    }
    else {
        QMessageBox::about(nullptr,"成功","同步信息配置成功！");
    }
    qDebug()<<Info;
}

//用来接收 源端配置发布 模式中添加的目的端用户
void MainWindow::infoRecv_source_peizhi_user_ip_port(QString ip_port)
{
    //将窗口传入的ip_port存入fabu_user_list

    source_peizhi_ip_port_list.append(ip_port);
    QMessageBox::about(nullptr,"成功","用户添加成功！");

    qDebug()<<ip_port;
}

//用来接收 目的端订阅发布 模式下添加的源端用户
void MainWindow::infoRecv_des_dingyue_ip_port(QString ip_port)
{
    //将窗口传入的ip_port存入dingyue_source_ip_port_list

    des_dingyue_source_ip_port_list = ip_port.split('_');

    QMessageBox::about(nullptr,"成功","用户添加成功！");
    qDebug()<<des_dingyue_source_ip_port_list;
}

//查询同步任务记录
void MainWindow::on_pushButton_clicked()
{
    if(!db.open()){
        QMessageBox::about(nullptr,"错误","未连接数据库!");
        return;
    }
    l = new transfer_log(this);
    l->show();

}

//查询错误记录
void MainWindow::on_pushButton_3_clicked()
{
    if(!db.open()){
        QMessageBox::about(nullptr,"错误","未连接数据库!");
        return;
    }
    e_l = new error_log(this);
    e_l->show();
}

//详细信息配置按钮
//根据用户身份和同步策略的不同显示不同的信息配置界面
void MainWindow::on_pushButton_test_clicked()
{
#ifndef DEBUG
    if(!db.open()){
        QMessageBox::about(nullptr,"错误","未连接数据库!");
        return;
    }
#endif
    QString user=ui->comboBox_character->currentText(),mode=ui->comboBox_mode->currentText();
    if(user == "源端" && mode == "订阅发布"){
        s_d = new source_dingyue(this);
        s_d->setWindowTitle("选择要发布的数据");
        connect(s_d,SIGNAL(infoSend(QString)),this,SLOT(infoRecv_source_dingyue_pattern_table(QString)));
        s_d->show();

    }
    else if(user == "源端" && mode == "配置发布"){
        s_p = new source_peizhi(this);
        s_p->setWindowTitle("选择要发布的数据");
        connect(s_p,SIGNAL(infoSend(QString)),this,SLOT(infoRecv_source_peizhi_pattern_table(QString)));
        connect(s_p,SIGNAL(infoSend_user(QString)),this,SLOT(infoRecv_source_peizhi_user_ip_port(QString)));
        s_p->show();

    }
    else if(user == "目的端" && mode == "订阅发布"){
        d_d = new destination_dingyue(this);
        d_d->setWindowTitle("选择要发布的数据");
        connect(d_d,SIGNAL(infoSend_user(QString)),this,SLOT(infoRecv_des_dingyue_ip_port(QString)));
        d_d->show();
    }
    else {//user == "目的端" && mode == "配置发布"；不需进行操作
        QMessageBox::about(nullptr,"成功","同步信息配置成功！");
    }
}

//同步状态监控
void MainWindow::on_pushButton_condition_monitor_clicked()
{
    d_l = new Dialog(this);
    d_l->show();
}

void MainWindow::on_pushButton_2_clicked()
{
//    n_d = new netWindow(this);
//    n_d->show();
}
