#pragma once

#include <QWidget>
#include <QTimer>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextcursor.h>

#include "live2dwidget/LAppDefine.hpp"
#include "live2dwidget/LAppPal.hpp"
#include "modelapi/OllamaApi.hpp"

using namespace LAppDefine;

class ChatWidget: public QWidget {
public:
    ChatWidget(QWidget *parent=nullptr):
        QWidget(parent) {
            chatContent = "";
            chatText = new QTextEdit(this);
            chatText->setGeometry(QRect(0, 0, 400, 250));
            chatText->setReadOnly(true);
            chatEdit = new QLineEdit(this);
            chatEdit->setGeometry(QRect(0, 250, 300, 50));
            sendButton = new QPushButton(this);
            sendButton->setText(QString::fromStdString("Send"));
            sendButton->setGeometry(QRect(300, 250, 100, 50));
            connect(sendButton, &QPushButton::released, this, &ChatWidget::SendRequest);
            QTimer *timer = new QTimer();
            connect(timer, &QTimer::timeout, this, &ChatWidget::UpdateChatContent);
            timer->start(500);
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
        OllamaApi::SendRequest(data);
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[Qt] send request");
        }
    }
};