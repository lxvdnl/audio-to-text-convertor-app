#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <thread>

#include "../../Plugins/AudioStreamRecording/audio-stream-recording.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

   private slots:
    void on_recordingButton_clicked();

   private:
    Ui::MainWindow *ui;
    audioStreamRecording *record;
    std::thread recThread;
    std::string fileName;
    bool recordingInProgress;
};
#endif  // MAINWINDOW_H
