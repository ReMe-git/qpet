#pragma once

#include <qboxlayout.h>
#include <qcoreapplication.h>
#include <qfont.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <QCoreApplication>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
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
    chatText->setReadOnly(true);
    chatText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QFont textFont = chatText->font();
    textFont.setPointSize(12);
    chatText->setFont(textFont);
    chatText->setFontPointSize(12);
    chatEdit = new QLineEdit(this);
    sendButton = new QPushButton(this);
    sendButton->setText(QString::fromStdString("  Send"));
    connect(sendButton, &QPushButton::released, this, &ChatWidget::SendRequest);
    hchatLayout = new QHBoxLayout();
    hchatLayout->setSpacing(3);
    hchatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    hchatLayout->setContentsMargins(0, 0, 0, 0);
    hchatLayout->addWidget(chatEdit);
    hchatLayout->addWidget(sendButton);
    vchatLayout = new QVBoxLayout();
    vchatLayout->setSpacing(3);
    vchatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    vchatLayout->setContentsMargins(0, 0, 0, 0);
    vchatLayout->addWidget(chatText);
    chatLayout = new QGridLayout(this);
    chatLayout->setSpacing(3);
    chatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->addLayout(vchatLayout, 0, 0, 2, 1);
    chatLayout->addLayout(hchatLayout, 2, 0, 1, 1);
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ChatWidget::UpdateChatContent);
    timer->start(100);
  }

 private:
  QTextEdit *chatText;
  QLineEdit *chatEdit;
  QPushButton *sendButton;
  QGridLayout *chatLayout;
  QVBoxLayout *vchatLayout;
  QHBoxLayout *hchatLayout;
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
        chatText->setMarkdown(QString::fromStdString(chatContent));
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
