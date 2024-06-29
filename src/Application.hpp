#pragma once

#include <QSystemTrayIcon>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "ChatWidget.hpp"
#include "live2dwidget/Live2dWidget.hpp"

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
