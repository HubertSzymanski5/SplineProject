#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plot(QVector<double> &x, QVector<double> &y, QVector<double> &vX, QVector<double> &vY){
    QPen pen;
    ui->QPlot->addGraph();
    ui->QPlot->graph(0)->setData(x, y);
    ui->QPlot->addGraph();
    ui->QPlot->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    ui->QPlot->graph(1)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssCircle, 5 ) );
    pen.setColor( QColor( 255, 0, 0 ) );
    ui->QPlot->graph(1)->setPen(pen);
    ui->QPlot->graph(1)->setData(vX, vY);
    ui->QPlot->replot();
}

void MainWindow::savePNG() {
    QString fileName("outplot.png");
    QFile file(fileName);

    if ( !file.open(QIODevice::WriteOnly))
        qDebug() << file.errorString();
    else
        ui->QPlot->savePng(fileName);

}

void MainWindow::setXAxis(double x0, double xn, std::string xname){
    // set Xrange
    ui->QPlot->xAxis->setRange(x0, xn);
    // set Xlabel
    ui->QPlot->xAxis->setLabel( xname.c_str() );
}

void MainWindow::setYAxis(double y0, double yn, std::string yname){
    // set Yrange
    ui->QPlot->yAxis->setRange(y0, yn);
    // set Ylabel
    ui->QPlot->yAxis->setLabel( yname.c_str() );
}

void MainWindow::clear(){
    ui->QPlot->removeGraph(ui->QPlot->graph(0));
}
