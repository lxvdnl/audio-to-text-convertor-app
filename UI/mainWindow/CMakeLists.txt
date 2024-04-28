set(mainWindowFiles
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)
add_library(mainWindow ${mainWindowFiles})

target_link_libraries(mainWindow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
