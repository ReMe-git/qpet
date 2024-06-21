#include <QApplication>
#include <QThread>
#include <cstdio>
#include <iostream>
#include <string>

#include "Application.hpp"
#include "modelapi/OllamaApi.hpp"

class OllamaWorker: public QThread {
    public:
    void run(void)
    {
        OllamaApi::Run();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Application App;
    OllamaWorker worker;
    App.SetupApp();
    worker.start();
    return app.exec();
}
