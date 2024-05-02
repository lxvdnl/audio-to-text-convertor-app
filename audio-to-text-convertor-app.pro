QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    UI/mainWindow/mainwindow.cpp \
    audio-to-text-convertor-app.cpp \
    Plugins/AudioStreamRecording/audio-stream-recording.cpp \
    UI/AudioStreamRecordingWindow/audiostreamrecordingwindow.cpp

HEADERS += \
    Plugins/AudioStreamRecording/audio-stream-recording.hpp \
    UI/AudioStreamRecordingWindow/audiostreamrecordingwindow.hpp \
    UI/mainWindow/mainwindow.hpp

FORMS += UI/AudioStreamRecordingWindow/audiostreamrecordingwindow.ui \
    UI/mainWindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
