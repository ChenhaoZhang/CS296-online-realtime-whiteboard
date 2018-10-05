#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(mouse_pressed()), this, SLOT(setStart()));
    connect(this, SIGNAL(mouse_released()), this, SLOT(setEnd()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPen pointPen(Qt::red);
    pointPen.setWidth(1);


    painter.setPen(pointPen);
    painter.drawLine(start, end);

}

void MainWindow::setStart() {
    start.setX(this->x);
    start.setY(this->y);
}

void MainWindow::setEnd() {
    end.setX(this->x);
    end.setY(this->y);
}

//void MainWindow::mouseMoveEvent(QMouseEvent *event) {

//    emit mouse_pos();
//}

void MainWindow::mousePressedEvent(QMouseEvent *event) {
    this->x = event->x();
    this->y = event->y();
    emit mouse_pressed();
}

void MainWindow::mouseReleasedEvent(QMouseEvent *event) {
    this->x = event->x();
    this->y = event->y();
    emit mouse_released();
}
