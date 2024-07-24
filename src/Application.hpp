#pragma once

#include <QSystemTrayIcon>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QThread>

#include "ChatWidget.hpp"
#include "live2dwidget/Live2dWidget.hpp"
#include "modelapi/OllamaApi.hpp"
#include "pipertts/tts.hpp"


class OllamaWorker : public QThread {
 public:
  void run(void) { OllamaApi::Run(); }
};

class PiperTTSWorker : public QThread {
 public:
  void run(void) { PiperTTS::Run(); }
};

class Application {
 public:
  int SetupApp(int argc, char **argv);

 private:
  QApplication *application;
  QMainWindow *mainWindow;
  QWidget *centralWidget;
  Live2dWidget *live2dWidget;
  ChatWidget *chatWidget;
  QGridLayout *appLayout;
  QSystemTrayIcon *systemTray;
  QMenu *trayMenu;
  QAction *chatAction;
  QAction *showAction;
  QAction *exitAction;
  PiperTTSWorker pipertts;
  OllamaWorker ollama;
};
