#include "Application.hpp"
#include "modelapi/OllamaApi.hpp"
#include "live2dwidget/LAppDelegate.hpp"

void Application::SetupApp(void)
{
    OllamaApi::InitApi("http://localhost:11434/api/chat", "qwen2:7b");
    mainWindow = new QMainWindow();
    mainWindow->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground);
    mainWindow->resize(300, 600);
    centralWidget = new QWidget(mainWindow);
    centralWidget->setObjectName(QString::fromUtf8("centralwidget"));
    centralWidget->setGeometry(QRect(0, 0,300, 600));
    live2dWidget = new Live2dWidget(centralWidget, mainWindow);
    live2dWidget->setObjectName(QString::fromUtf8("openGLWidget"));
    live2dWidget->setGeometry(QRect(0, 0,300, 600));
    mainWindow->setCentralWidget(centralWidget);
    systemTray = new QSystemTrayIcon(mainWindow);
    systemTray->setVisible(true);
    mainWindow->show();
    std::string iconPath;
    iconPath += LAppDelegate::GetInstance()->GetExecuteAbsolutePath() + "Resources/icon.png";
    systemTray->setIcon(QIcon(iconPath.c_str()));
}