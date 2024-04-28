#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    record = new audioStreamRecording("recorded-stream.wav");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startRecording_clicked()
{
    record->Record();
}

void MainWindow::on_stopRecording_clicked()
{
    record->Stop();
}
