#include "Live2dWidget.hpp"

#include <qbuffer.h>

#include <QTimer>

#include "../modelapi/PiperTTSApi.hpp"
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppPal.hpp"
#include "OpenGLExtraFunctions.hpp"

using namespace LAppDefine;

float Live2dWidget::ratio;
QElapsedTimer Live2dWidget::elapsedTimer;

Live2dWidget::Live2dWidget(QWidget *parent)
    : QOpenGLWidget(parent) {
  this->setAttribute(Qt::WA_AlwaysStackOnTop);
  this->setAttribute(Qt::WA_TranslucentBackground);

  elapsedTimer.start();

  // Add the render into Timer to make the animation
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Live2dWidget::updateMotions);
  timer->start(1);

  // the OpenGL in OpenGLWidget won't scale by HDPI setting, we need to scale
  // manually.
  ratio = parent->devicePixelRatio();

  audioPlayer = new QMediaPlayer(this);
  audioOutput = new QAudioOutput(this);
  audioPlayer->setAudioOutput(audioOutput);
}
void Live2dWidget::initializeGL() {
  this->makeCurrent();
  OpenGLExtraFunctions::InitInstance(this->context());

  LAppDelegate::GetInstance()->Initialize(this);
  LAppDelegate::GetInstance()->Run();

  OpenGLExtraFunctions::GetInstance()->glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  OpenGLExtraFunctions::GetInstance()->glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  OpenGLExtraFunctions::GetInstance()->glEnable(GL_BLEND);
  OpenGLExtraFunctions::GetInstance()->glBlendFunc(GL_SRC_ALPHA,
                                                   GL_ONE_MINUS_SRC_ALPHA);
}
void Live2dWidget::resizeGL(int width, int height) {
  LAppDelegate::GetInstance()->Resize(width * ratio, height * ratio);
}

void Live2dWidget::paintGL() { LAppDelegate::GetInstance()->Run(); }

void Live2dWidget::clear() {
  OpenGLExtraFunctions::GetInstance()->glViewport(0, 0, width() * ratio,
                                                  height() * ratio);
  OpenGLExtraFunctions::GetInstance()->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  OpenGLExtraFunctions::GetInstance()->glClear(GL_COLOR_BUFFER_BIT |
                                               GL_DEPTH_BUFFER_BIT);
  OpenGLExtraFunctions::GetInstance()->glClearDepthf(1.0);
}

void Live2dWidget::updateMotions() {
  std::vector<char> wavData;
  QMediaPlayer::MediaStatus status = audioPlayer->mediaStatus();
  if (status == QMediaPlayer::NoMedia || status == QMediaPlayer::EndOfMedia) {
    if (PiperTTSApi::GetRespone(wavData)) {
      if (DebugLogEnable) {
        LAppPal::PrintLogLn("[LIVE2D]get wavdata, size is %d", wavData.size());
      }
      LAppDelegate::GetInstance()->GetWavFileHandler()->Start(wavData);
      QByteArray audioData(wavData.data(), wavData.size());
      QBuffer *audioBuffer = new QBuffer(this);
      audioBuffer->setData(audioData);
      audioBuffer->open(QIODevice::ReadOnly);
      audioPlayer->stop();
      audioPlayer->setSourceDevice(audioBuffer);
      audioPlayer->setPlaybackRate(1.25);
      audioPlayer->play();
    }
  }
  update();
}

void Live2dWidget::mousePressEvent(QMouseEvent *event) {
  LAppDelegate::GetInstance()->mousePressEvent(event);
}  // mousePressEvent

void Live2dWidget::mouseMoveEvent(QMouseEvent *event) {
  LAppDelegate::GetInstance()->mouseMoveEvent(event);
}  // mouseMoveEvent

void Live2dWidget::mouseReleaseEvent(QMouseEvent *event) {
  LAppDelegate::GetInstance()->mouseReleaseEvent(event);
}  // mouseReleaseEvent
