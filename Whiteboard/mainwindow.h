#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QPen>
#include <QMouseEvent>
#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;

protected:
    void paintEvent(QMouseEvent *event);
    void mousePressedEvent(QMouseEvent *event);
    void mouseReleasedEvent(QMouseEvent *event);
    void paint(QPaintEvent *event);
    int x,y;
    QPoint start;
    QPoint end;
    bool pressed = false;

public slots:
    void setStart();
    void setEnd();


signals:
    void mouse_pressed();
    void mouse_pos();
    void mouse_released();

};

#endif // MAINWINDOW_H
