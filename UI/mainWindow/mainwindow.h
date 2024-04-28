#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../../Plugins/AudioStreamRecording/audio-stream-recording.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startRecording_clicked();

    void on_stopRecording_clicked();

private:
    Ui::MainWindow *ui;
    audioStreamRecording *record;
};
#endif // MAINWINDOW_H
