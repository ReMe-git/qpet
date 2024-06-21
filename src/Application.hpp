#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>

#include "live2dwidget/Live2dWidget.hpp"
#include "ChatWidget.hpp" 
#include "modelapi/OllamaApi.hpp"

class Application {
public:
    void SetupApp(void);

private:
    QMainWindow *mainWindow;
    QWidget *centralWidget;
    Live2dWidget *live2dWidget;
    ChatWidget *chatWidget;
    QSystemTrayIcon *systemTray;
};