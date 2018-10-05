#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QPen>
#include <QMouseEvent>

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
    void paintEvent(QPaintEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    void mousePressedEvent(QMouseEvent *event);
    void mouseReleasedEvent(QMouseEvent *event);
    //void setStart();
    //void setEnd();
    int x,y;
    QPoint start;
    QPoint end;

public slots:
    void setStart();
    void setEnd();

signals:
    bool mouse_pressed();
    void mouse_pos();
    bool mouse_released();

};

#endif // MAINWINDOW_H
