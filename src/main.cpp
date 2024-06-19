#include <QApplication>
#include "live2dwidget/LAppDelegate.hpp"
#include "ui.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Ui::MainWindow win;

    win.setupUi();
    win.show();
    std::string iconPath;
    iconPath += LAppDelegate::GetInstance()->GetExecuteAbsolutePath() + "Resources/icon.png";
    win.tray->setIcon(QIcon(iconPath.c_str()));

    return app.exec();
}
