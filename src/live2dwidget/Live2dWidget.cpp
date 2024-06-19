#include "Live2dWidget.hpp"
#include "OpenGLExtraFunctions.hpp"
#include <QApplication>
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppPal.hpp"
#include <QTimer>
#include <qmainwindow.h>
#include <qnamespace.h>

using namespace LAppDefine;

Live2dWidget::Live2dWidget(QWidget *parent, QMainWindow *mainWindow):
    QOpenGLWidget(parent)
{
    this->mainWindow = mainWindow;
    this->setAttribute(Qt::WA_AlwaysStackOnTop);
    this->setAttribute(Qt::WA_TranslucentBackground);

    elapsedTimer.start();

    //Add the render into Timer to make the animation
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Live2dWidget::updateMotions);
    timer->start(1);

    // the OpenGL in OpenGLWidget won't scale by HDPI setting, we need to scale manually.
    ratio = parent->devicePixelRatio();
}
void Live2dWidget::initializeGL()
{
    this->makeCurrent();
    OpenGLExtraFunctions::InitInstance(this->context());

    LAppDelegate::GetInstance()->Initialize(this);
    LAppDelegate::GetInstance()->Run();

    OpenGLExtraFunctions::GetInstance()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    OpenGLExtraFunctions::GetInstance()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    OpenGLExtraFunctions::GetInstance()->glEnable(GL_BLEND);
    OpenGLExtraFunctions::GetInstance()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
void Live2dWidget::resizeGL(int width, int height)
{
    LAppDelegate::GetInstance()->Resize(width*ratio,height*ratio);
}

void Live2dWidget::paintGL()
{
    LAppDelegate::GetInstance()->Run();
}

void Live2dWidget::clear()
{
    OpenGLExtraFunctions::GetInstance()->glViewport(0, 0, width()*ratio, height()*ratio);
    OpenGLExtraFunctions::GetInstance()->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    OpenGLExtraFunctions::GetInstance()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OpenGLExtraFunctions::GetInstance()->glClearDepthf(1.0);
}


void Live2dWidget::updateMotions()
{
    update();
}

void Live2dWidget::mousePressEvent(QMouseEvent * event){
    if (event->button() == Qt::LeftButton && !m_isMove) {
        m_isMove = true;
        last = event->globalPosition().toPoint();
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[Qt]mouse press: {x:%.2f, y:%.2f}", 
                event->globalPosition().x(), event->globalPosition().y());
        }
    }
    LAppDelegate::GetInstance()->mousePressEvent(event);
} //mousePressEvent

void Live2dWidget::mouseMoveEvent(QMouseEvent * event){
     if (m_isMove) {
        int dx = event->globalPosition().x() - last.x();
        int dy = event->globalPosition().y() - last.y();
        last = event->globalPosition().toPoint();
        QMainWindow *wp = this->mainWindow;
        wp->move(wp->x() + dx, wp->y() + dy);
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[Qt]mouse move: {x:%.2f, y:%.2f}", 
                event->globalPosition().x(), event->globalPosition().y());
        }
    }
    LAppDelegate::GetInstance()->mouseMoveEvent(event);
} //mouseMoveEvent
void Live2dWidget::mouseReleaseEvent(QMouseEvent * event){
    if (m_isMove) {
        int dx = event->globalPosition().x() - last.x();
        int dy = event->globalPosition().y() - last.y();
        QMainWindow *wp = this->mainWindow;
        wp->move(wp->x() + dx, wp->y() + dy);
        m_isMove = false;
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[Qt]mouse release: {x:%.2f, y:%.2f}", 
                event->globalPosition().x(), event->globalPosition().y());
        }
    }
    LAppDelegate::GetInstance()->mouseReleaseEvent(event);
} // mouseReleaseEvent