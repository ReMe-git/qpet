#include <QApplication>
#include "ui.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Ui::MainWindow win;

    win.setupUi();
    win.show();
    return app.exec();
}
