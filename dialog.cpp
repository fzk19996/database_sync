#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::paintEvent(QPaintEvent *){
    //生成Qlist数据
    getInput();
    //设置绘图参数
    int lineLength = 200; //连接线的大小
    int rectWidth = 100; //ip地址框宽度
    int rectHeight = 20; //ip地址框高度
    int sparseLength = 30; //线段与图片中心的距离
    int getPcNum = connectLists.size();//连接点数量
    //设置划线类型
    QPen connectRed, connectYellow, connectGreen;
    connectRed.setColor("red");
    connectRed.setWidth(3);

    connectYellow.setColor("yellow");
    connectYellow.setWidth(3);

    connectGreen.setColor("green");
    connectGreen.setWidth(3);
    /////////////利用坐标转换 以中心点为圆心画圆///////////////

    QPainter painter(this);//在当前Widget画图，也可以自定义

    ///////////对图片进行缩放////////////
    //导入服务器和PC图片
    QPixmap serverPix, pcPix;
    serverPix.load(":/Image/serverNew.png");
    pcPix.load(":/Image/pcNew.png");
    //对两张图片进行缩放
    serverPix = serverPix.scaled(serverPix.width()/5, serverPix.height()/5, Qt::KeepAspectRatio);
    pcPix = pcPix.scaled(pcPix.width()/5, pcPix.height()/5, Qt::KeepAspectRatio);
    //获取图片宽高值用于绘图中心点处理
    int serverPixWidth = serverPix.width();
    int serverPixHeight = serverPix.height();

    int pcPixWidth = pcPix.width();
    int pcPixHeight = pcPix.height();

    //////////////开始绘图////////////
    //将坐标轴平移到屏幕中央
    painter.translate(this->width()/2, this->height()/2);
    //从屏幕中心点开始，逆时针旋转画连接线；
    for(int i=0;i<getPcNum;i++){
        //保存画笔状态为屏幕中心
        painter.save();

        //获取QList数据
        pcInfo getPcInfo = connectLists[i];
        int lineColor = getPcInfo.lineColor;
        QString ipaddr = getPcInfo.ipaddr;

        //设置画笔颜色
        if(lineColor==0) {painter.setPen(connectRed);}
        else if (lineColor==1) {painter.setPen(connectYellow);}
        else if (lineColor==2) {painter.setPen(connectGreen);}
        painter.rotate(360/getPcNum*i);
        //开始绘制箭头直线
        painter.drawLine(0, 0, lineLength-sparseLength, 0);
        painter.translate(lineLength-sparseLength, 0);
        painter.drawLine(0, 0, -5, 5);
        painter.drawLine(0, 0, -5, -5);
        painter.translate(sparseLength, 0);

        painter.rotate(-360/getPcNum*i);

        //绘子节点（pc）图像
        painter.drawPixmap(-pcPixWidth/2, -pcPixHeight/2, pcPix);
        //为了不让ip地址框盖住连接线，因此将上半部分的地址框转移到图像上方
        QRect rectbelow(-rectWidth/2, pcPixHeight/2, rectWidth, rectHeight);
        QRect rectAbove(-rectWidth/2, -pcPixHeight/2-rectHeight, rectWidth, rectHeight);
        painter.setPen("black");
        if(360/getPcNum*i < 180){
            painter.drawText(rectbelow, Qt::AlignHCenter, ipaddr);
        }
        else {
            painter.drawText(rectAbove, Qt::AlignHCenter, ipaddr);
        }
        painter.restore();
    }
        //画母节点图
    painter.drawPixmap(-serverPixWidth/2, -serverPixHeight/2, serverPix);
    connectLists.clear();
}

void Dialog::getInput(){

    pcInfo perPc;
    for(int i=0;i<setPcNum;i++){
        if(i%3 == 0){perPc.lineColor = 0;}
        if(i%3 == 1){perPc.lineColor = 1;}
        if(i%3 == 2){perPc.lineColor = 2;}
        QString ip = QString("%1.%2.%3.%4").arg(192).arg(168).arg(1).arg(i);
        perPc.ipaddr = ip;
        connectLists.append(perPc);
    }
}




void Dialog::on_btn_refresh_clicked()
{
    setPcNum +=1;
    repaint();
}
