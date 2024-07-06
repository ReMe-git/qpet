#include <QThread>

#include "Application.hpp"
#include "modelapi/OllamaApi.hpp"
#include "modelapi/PiperTTSApi.hpp"


int main(int argc, char *argv[]) {
  Application App;
  return App.SetupApp(argc, argv);
}
