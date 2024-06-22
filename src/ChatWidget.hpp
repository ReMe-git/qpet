#pragma once

#include <QWidget>
#include <QTimer>
#include <QTextEdit>
#include <QScrollBar>
#include <QPushButton>
#include <QLineEdit>

#include "live2dwidget/LAppDefine.hpp"
#include "live2dwidget/LAppPal.hpp"
#include "modelapi/OllamaApi.hpp"

using namespace LAppDefine;

class ChatWidget: public QWidget {
public:
    ChatWidget(QWidget *parent=nullptr):
        QWidget(parent) {
            //this->setStyleSheet(QString::fromUtf8("background-color: rgb(26, 27, 38);color: rgb(169, 177, 214);"));
            this->setStyleSheet(QString::fromUtf8("border: 2px solid grey;background-color: #32CC99;color: #FFFFFF"));
            chatContent = "";
            chatText = new QTextEdit(this);
            chatText->setGeometry(QRect(0, 0, 400, 250));
            chatText->setReadOnly(true);
            chatText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            chatText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            chatText->verticalScrollBar()->setStyleSheet(" QScrollBar:vertical { \
                                                                border: 2px solid grey; \
                                                                background: #32CC99; \
                                                                width: 15px; \
                                                                margin: 22px 0 22px 0; \
                                                            } \
                                                            QScrollBar::handle:vertical { \
                                                                background: white; \
                                                                min-height: 20px; \
                                                            } \
                                                            QScrollBar::add-line:vertical { \
                                                                border: 2px solid grey; \
                                                                background: #32CC99; \
                                                                height: 20px; \
                                                                subcontrol-position: bottom; \
                                                                subcontrol-origin: margin; \
                                                            } \
                                                            QScrollBar::sub-line:vertical { \
                                                                border: 2px solid grey; \
                                                                background: #32CC99; \
                                                                height: 20px; \
                                                                subcontrol-position: top; \
                                                                subcontrol-origin: margin; \
                                                            } \
                                                            QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { \
                                                                border: 2px solid grey; \
                                                                width: 3px; \
                                                                height: 3px; \
                                                                background: white; \
                                                            } \
                                                            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { \
                                                                background: none; \
                                                            }");
            chatEdit = new QLineEdit(this);
            chatEdit->setGeometry(QRect(0, 250, 300, 50));
            sendButton = new QPushButton(this);
            sendButton->setText(QString::fromStdString("Send"));
            sendButton->setGeometry(QRect(300, 250, 100, 50));
            sendButton->setStyleSheet("QPushButton:pressed{color: #FFFFFF; background-color:#54AD8F;}");
            connect(sendButton, &QPushButton::released, this, &ChatWidget::SendRequest);
            QTimer *timer = new QTimer();
            connect(timer, &QTimer::timeout, this, &ChatWidget::UpdateChatContent);
            timer->start(100);
    }

private:
    QTextEdit* chatText;
    QLineEdit* chatEdit;
    QPushButton* sendButton;
    std::string chatContent;
private slots:
    void UpdateChatContent(void)
    {
        std::string data;
        if (OllamaApi::GetRespone(data)) {
            chatContent += data;
            chatText->setPlainText(QString::fromStdString(chatContent));
            chatText->moveCursor(QTextCursor::End);
            if (DebugLogEnable) {
                LAppPal::PrintLogLn("[Qt]update chat content");
            }
        }
    }

    void SendRequest(void)
    {
        std::string data;
        data = chatEdit->text().toStdString();
        chatEdit->setText("");
        if (data.length() > 1) {
            OllamaApi::SendRequest(data);
            if (DebugLogEnable) {
                LAppPal::PrintLogLn("[Qt]send request");
            }
        }
    }
};