#pragma once

#include <QAudioOutput>
#include <QBuffer>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class Live2dWidget : public QOpenGLWidget {
 public:
  Live2dWidget(QWidget *parent = nullptr, QMainWindow *mainWindow = nullptr);
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
  void clear();

  static float ratio;
  static QElapsedTimer elapsedTimer;

 protected:
  bool m_isPress;
  QPoint m_lastPos;
  QMainWindow *mainWindow;
  QMediaPlayer *audioPlayer;
  QAudioOutput *audioOutput;

  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private slots:
  void updateMotions();
};
