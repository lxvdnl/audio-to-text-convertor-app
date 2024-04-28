set(mainWindowFiles
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
)
add_library(mainWindow ${mainWindowFiles})

target_link_libraries(mainWindow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
