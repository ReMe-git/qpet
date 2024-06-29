#include <QApplication>
#include <QThread>

#include "Application.hpp"
#include "modelapi/OllamaApi.hpp"
#include "modelapi/PiperTTSApi.hpp"

class OllamaWorker : public QThread {
 public:
  void run(void) { OllamaApi::Run(); }
};

class PiperTTSWorker : public QThread {
 public:
  void run(void) { PiperTTSApi::Run(); }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Application App;
  OllamaWorker ollamaWorker;
  PiperTTSWorker piperTTSWorker;
  App.SetupApp();
  ollamaWorker.start();
  piperTTSWorker.start();
  return app.exec();
}
