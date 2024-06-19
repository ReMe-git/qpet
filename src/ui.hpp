/********************************************************************************
** Form generated from reading UI file 'designerazLPbe.ui'
**
** Created by: Qt User Interface Compiler version 5.15.14
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DESIGNERAZLPBE_H
#define DESIGNERAZLPBE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>
#include <qevent.h>
#include <qnamespace.h>
#include "live2dwidget/Live2dWidget.hpp"
#include "live2dwidget/LAppPal.hpp"

QT_BEGIN_NAMESPACE

class Ui_MainWindow : public QMainWindow
{
public:
    QWidget *centralwidget;
    QOpenGLWidget *openGLWidget;
    QSystemTrayIcon *tray;

    void setupUi(void)
    {
        this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setStyleSheet("background:transparent;");
        if (this->objectName().isEmpty())
            this->setObjectName(QString::fromUtf8("qpet"));
        this->resize(300, 600);
        centralwidget = new QWidget(this);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        openGLWidget = new Live2dWidget(centralwidget, this);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0,300, 600));
        this->setCentralWidget(centralwidget);
        tray = new QSystemTrayIcon(this);
        tray->setVisible(true);
        retranslateUi();

        QMetaObject::connectSlotsByName(this);
    } // setupUi

    void retranslateUi(void)
    {
        this->setWindowTitle(QCoreApplication::translate("qpet", "qpet", nullptr));
    } // retranslateUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DESIGNERAZLPBE_H
