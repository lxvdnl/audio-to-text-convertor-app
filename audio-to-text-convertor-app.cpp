#include <QApplication>

#include "UI/mainWindow/mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication audio_to_text_convertor_app(argc, argv);
    MainWindow mainwindow;
    mainwindow.show();
    return audio_to_text_convertor_app.exec();
}
