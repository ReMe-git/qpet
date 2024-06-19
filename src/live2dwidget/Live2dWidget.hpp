#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <qmainwindow.h>
#include <qpoint.h>

class Live2dWidget: public QOpenGLWidget {
public:
    Live2dWidget(QWidget *parent = nullptr, QMainWindow *mainWindow = nullptr);
    void initializeGL()override;
    void resizeGL(int width, int height)override;
    void paintGL()override;
    void clear();
    
    static inline float ratio;
    static inline QElapsedTimer elapsedTimer;

protected:
    bool m_isMove;
    QPoint last;
    QMainWindow *mainWindow;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateMotions();
};
