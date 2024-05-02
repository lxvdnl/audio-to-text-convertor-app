#include "audiostreamrecordingwindow.hpp"

#include "ui_audiostreamrecordingwindow.h"

AudioStreamRecordingWindow::AudioStreamRecordingWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AudioStreamRecordingWindow)
{
    ui->setupUi(this);
    fileName = "recorded-stream.wav";
    record = new audioStreamRecording(fileName);
    recordingInProgress = false;
}

AudioStreamRecordingWindow::~AudioStreamRecordingWindow()
{
    delete ui;
}

void AudioStreamRecordingWindow::on_recordingButton_clicked()
{
    if (!recordingInProgress) {
        ui->recordingButton->setText("Stop Recording");
        ui->recordingButton->setStyleSheet(
            "background-color: #f54242; color: #ffffff;");

        recThread = std::thread(&audioStreamRecording::Record, record);
        recThread.detach();

        recordingInProgress = true;
    } else {
        record->Stop();

        ui->recordingButton->setEnabled(false);
        ui->recordingButton->setText("Audio recorded successfully");
        ui->recordingButton->setStyleSheet(
            "background-color: #7d7575; color: #ffffff;");

        recordingInProgress = false;
    }
}

void AudioStreamRecordingWindow::closeEvent(QCloseEvent *event)
{
    if (recordingInProgress) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this, "Recording in Progress",
            "Recording is still in progress. Do you want to stop "
            "recording and close the application?",
            QMessageBox::Yes | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            record->Stop();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }

    event->accept();
}
