#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // konstruktory
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // interface
    void plot(QVector<double> &x, QVector<double> &y, QVector<double> &vX, QVector<double> &vY);
    void savePNG();
    void setXAxis(double x0, double xn, std::string xname);
    void setYAxis(double y0, double yn, std::string yname);
    void clear();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
