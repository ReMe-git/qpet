#include "Application.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QWidget>
#include <QMenu>
#include <QAction>

#include "spdlog/spdlog.h"
#include "modelapi/OllamaApi.hpp"
#include "pipertts/tts.hpp"

int Application::SetupApp(int argc, char **argv) {
  application = new QApplication(argc, argv);
	spdlog::set_level(spdlog::level::debug);
  OllamaApi::InitApi("http://10.0.209.254:11434/api/chat", "qwen2:1.5b");
  //PiperTTSApi::InitApi("http://localhost:8080/tts", "voice-zh-cn-huayan-x-low",
  //                     "voice-en-us-amy-low");
	std::string enModelPath = QCoreApplication::applicationDirPath().toStdString() +
		"/res/piper/models/en_US-amy-medium.onnx";
	std::string zhModelPath = QCoreApplication::applicationDirPath().toStdString() +
		"/res/piper/models/zh_CN-huayan-medium.onnx";
	std::string eSpeakDataPath = QCoreApplication::applicationDirPath().toStdString() +
		"/res/piper/espeak-ng-data";
	PiperTTS::Initialize(zhModelPath, 0, enModelPath, 0, eSpeakDataPath, false);
  mainWindow = new QMainWindow();
  QFile styleFile(QCoreApplication::applicationDirPath() +
                  "/res/ui/style.qss");
  styleFile.open(QIODevice::ReadOnly);
  mainWindow->setStyleSheet(styleFile.readAll());
  styleFile.close();
  mainWindow->setWindowFlags(Qt::WindowStaysOnTopHint |
                             Qt::FramelessWindowHint | Qt::Tool);
  mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
  mainWindow->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/res/ui/icon.png"));
  mainWindow->setFixedSize(650, 600);
  centralWidget = new QWidget(mainWindow);
  centralWidget->setFixedSize(650, 600);
  live2dWidget = new Live2dWidget(centralWidget);
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
      QIcon(QCoreApplication::applicationDirPath() + "/res/ui/icon.png"));
  trayMenu = new QMenu();
  chatAction = new QAction("󰭻  Chat", trayMenu);
  showAction = new QAction("󰈈  Show", trayMenu);
  exitAction = new QAction("󰅙  Exit", trayMenu);
  trayMenu->addAction(chatAction);
  trayMenu->addAction(showAction);
	trayMenu->addAction(exitAction);
  mainWindow->connect(chatAction, &QAction::triggered, [this]() {
    if (!chatWidget->isVisible()) { chatWidget->setVisible(true); }
    else { chatWidget->setVisible(false); }
  });
	mainWindow->connect(showAction, &QAction::triggered, [this]() {
			if (!mainWindow->isVisible()) { mainWindow->setVisible(true); }
			else { mainWindow->setVisible(false); }
	});
  mainWindow->connect(exitAction, &QAction::triggered, [this]() {
    ollama.terminate();
    ollama.wait();
    pipertts.terminate();
    pipertts.wait();
		PiperTTS::Release();
    application->exit();
  });
  systemTray->setContextMenu(trayMenu);
  mainWindow->show();
  ollama.start();
  pipertts.start();
  return application->exec();
}
