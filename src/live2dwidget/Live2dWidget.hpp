#pragma once

#include <QAudioOutput>
#include <QBuffer>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class Live2dWidget : public QOpenGLWidget {
 public:
  Live2dWidget(QWidget *parent = nullptr);
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
  void clear();

  static float ratio;
  static QElapsedTimer elapsedTimer;

 protected:
  QMediaPlayer *audioPlayer;
  QAudioOutput *audioOutput;

  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private slots:
  void updateMotions();
};
