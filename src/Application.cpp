#include "Application.hpp"
#include <qaction.h>
#include <qapplication.h>

#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QWidget>
#include <QMenu>
#include <QAction>

#include "modelapi/OllamaApi.hpp"
#include "modelapi/PiperTTSApi.hpp"

int Application::SetupApp(int argc, char **argv) {
  application = new QApplication(argc, argv);
  OllamaApi::InitApi("http://localhost:11434/api/chat", "qwen2:7b");
  PiperTTSApi::InitApi("http://localhost:8080/tts", "voice-zh_CN-huayan-medium",
                       "voice-en-us-amy-low");
  mainWindow = new QMainWindow();
  QFile styleFile(QCoreApplication::applicationDirPath() +
                  "/Resources/style.qss");
  styleFile.open(QIODevice::ReadOnly);
  mainWindow->setStyleSheet(styleFile.readAll());
  styleFile.close();
  mainWindow->setWindowFlags(Qt::WindowStaysOnTopHint |
                             Qt::FramelessWindowHint | Qt::Tool);
  mainWindow->setAttribute(Qt::WA_TranslucentBackground);
  mainWindow->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/Resources/icon.png"));
  mainWindow->setFixedSize(650, 600);
  centralWidget = new QWidget(mainWindow);
  centralWidget->setObjectName(QString::fromUtf8("centralwidget"));
  live2dWidget = new Live2dWidget(centralWidget);
  live2dWidget->setObjectName(QString::fromUtf8("openGLWidget"));
  chatWidget = new ChatWidget(mainWindow);
  mainWindow->setCentralWidget(centralWidget);
  appLayout = new QGridLayout(centralWidget);
  appLayout->setSpacing(0);
  appLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
  appLayout->setContentsMargins(0, 0, 0, 0);
  appLayout->addWidget(chatWidget, 30, 0, 50, 60);
  appLayout->addWidget(live2dWidget, 0, 60, 100, 40);
  systemTray = new QSystemTrayIcon(mainWindow);
  systemTray->setVisible(true);
  systemTray->setIcon(
      QIcon(QCoreApplication::applicationDirPath() + "/Resources/icon.png"));
  trayMenu = new QMenu();
  QAction *showAction = new QAction("Show", trayMenu);
  QAction *hideAction = new QAction("Hide", trayMenu);
  QAction *exitAction = new QAction("Exit", trayMenu);
  trayMenu->addAction(showAction);
  trayMenu->addAction(hideAction);
  trayMenu->addAction(exitAction);
  mainWindow->connect(showAction, &QAction::triggered, [this]() {
    if (!mainWindow->isVisible()) mainWindow->setVisible(true);
  });
  mainWindow->connect(hideAction, &QAction::triggered, [this]() {
    if (mainWindow->isVisible()) mainWindow->setVisible(false);
  });
  mainWindow->connect(exitAction, &QAction::triggered, [this]() {
    ollama.terminate();
    ollama.wait();
    pipertts.terminate();
    pipertts.wait();
    application->exit();
  });
  systemTray->setContextMenu(trayMenu);
  mainWindow->show();
  ollama.start();
  pipertts.start();
  return application->exec();
}
