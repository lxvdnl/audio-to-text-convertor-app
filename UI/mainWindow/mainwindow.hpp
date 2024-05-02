#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "../audioStreamRecordingWindow/audiostreamrecordingwindow.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_goToTheRecordingButton_clicked();

   private:
    Ui::MainWindow *ui;
    AudioStreamRecordingWindow *audiostreamrecordingwindow;
};

#endif  // MAINWINDOW_HPP
