#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(mouse_pressed()), this, SLOT(setStart()));
    connect(this, SIGNAL(mouse_released()), this, SLOT(setEnd()));
    //connect(this, SIGNAL(mouse_pressed()), this, SLOT(paintEvent(QPaintEvent *event)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QMouseEvent *event) {

        QPainter painter(this);
        QPen pointPen(Qt::red);
        pointPen.setWidth(1);

//        QPoint a;
//        a.setX(this->x);
//        a.setY(this->y);

//        QPoint b;
//        b.setX(100);
//        b.setY(100);
        this->start.setX(20);
        this->start.setY(20);
        this->end.setX(50);
        this->end.setY(50);

        painter.setPen(pointPen);
        painter.drawLine(this->start, this->end);
        //painter.drawLine(a, b);



}

void MainWindow::setStart() {
    start.setX(this->x);
    start.setY(this->y);

}

void MainWindow::setEnd() {
    end.setX(this->x);
    end.setY(this->y);
    QWidget::update();
}

void MainWindow::mousePressedEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        this->start.setX(event->x());
        this->start.setY(event->y());
        //emit mouse_pressed();
        this->pressed = true;
    }
}

void MainWindow::mouseReleasedEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->end.setX(event->x());
        this->end.setY(event->y());
        emit mouse_released();
    }
}
