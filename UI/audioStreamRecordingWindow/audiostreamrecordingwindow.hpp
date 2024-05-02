#ifndef AUDIOSTREAMRECORDINGWINDOW_HPP
#define AUDIOSTREAMRECORDINGWINDOW_HPP

#include <QCloseEvent>
#include <QDialog>
#include <QMessageBox>
#include "../../Plugins/AudioStreamRecording/audio-stream-recording-win.hpp"
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui {
class AudioStreamRecordingWindow;
}
QT_END_NAMESPACE

class AudioStreamRecordingWindow : public QDialog
{
    Q_OBJECT

public:
    AudioStreamRecordingWindow(QWidget *parent = nullptr);
    ~AudioStreamRecordingWindow();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_recordingButton_clicked();

private:
    Ui::AudioStreamRecordingWindow *ui;
    audioStreamRecordingWin *record;
    std::thread recThread;
    std::string fileName;
    bool recordingInProgress;
};

#endif // AUDIOSTREAMRECORDINGWINDOW_HPP
