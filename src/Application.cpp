#include "Application.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QWidget>

#include "modelapi/OllamaApi.hpp"
#include "modelapi/PiperTTSApi.hpp"

void Application::SetupApp(void) {
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
                             Qt::FramelessWindowHint);
  mainWindow->setAttribute(Qt::WA_TranslucentBackground);
  mainWindow->resize(700, 600);
  centralWidget = new QWidget(mainWindow);
  centralWidget->setObjectName(QString::fromUtf8("centralwidget"));
  centralWidget->setGeometry(QRect(400, 0, 200, 600));
  live2dWidget = new Live2dWidget(centralWidget);
  live2dWidget->setObjectName(QString::fromUtf8("openGLWidget"));
  live2dWidget->setGeometry(QRect(400, 0, 300, 600));
  chatWidget = new ChatWidget(centralWidget);
  chatWidget->setGeometry(QRect(0, 100, 400, 300));
  mainWindow->setCentralWidget(centralWidget);
  systemTray = new QSystemTrayIcon(mainWindow);
  systemTray->setVisible(true);
  systemTray->setIcon(
      QIcon(QCoreApplication::applicationDirPath() + "/Resources/icon.png"));
  mainWindow->show();
}
