#pragma once

#include <QCoreApplication>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <string>

#include "live2dwidget/LAppDefine.hpp"
#include "live2dwidget/LAppPal.hpp"
#include "modelapi/OllamaApi.hpp"
#include "modelapi/PiperTTSApi.hpp"
#include "modelapi/TextParser.hpp"

using namespace LAppDefine;

class ChatWidget : public QWidget {
 public:
  ChatWidget(QWidget *parent = nullptr) : QWidget(parent) {
    chatText = new QTextEdit(this);
    chatText->setGeometry(QRect(0, 0, 400, 250));
    chatText->setReadOnly(true);
    chatText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatEdit = new QLineEdit(this);
    chatEdit->setGeometry(QRect(0, 250, 300, 50));
    sendButton = new QPushButton(this);
    sendButton->setText(QString::fromStdString("Send"));
    sendButton->setGeometry(QRect(300, 250, 100, 50));
    connect(sendButton, &QPushButton::released, this, &ChatWidget::SendRequest);
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ChatWidget::UpdateChatContent);
    timer->start(100);
  }

 private:
  QTextEdit *chatText;
  QLineEdit *chatEdit;
  QPushButton *sendButton;
  std::string chatContent;
  TextParser chatParser;

 private slots:
  void UpdateChatContent(void) {
    std::string data;
    if (OllamaApi::GetRespone(data)) {
      chatContent += data;
      chatParser.AppendText(data);
      chatParser.SPlitTextByPunctuation();
      TextStruct newText;
      if (chatParser.GetText(newText)) {
        chatText->setPlainText(QString::fromStdString(chatContent));
        chatText->moveCursor(QTextCursor::End);
        std::string content;
        newText.GetContent(content);
        PiperTTSApi::SendRequest(content);
        if (DebugLogEnable) {
          LAppPal::PrintLogLn("[Qt]update chat content");
        }
      }
    }
  }

  void SendRequest(void) {
    std::string data;
    data = chatEdit->text().toStdString();
    chatEdit->clear();
    if (data.length() > 0) {
      OllamaApi::SendRequest(data);
      if (DebugLogEnable) {
        LAppPal::PrintLogLn("[Qt]send request");
      }
    }
  }
};
