#pragma once

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

#include "spdlog/spdlog.h"
#include "modelapi/OllamaApi.hpp"
#include "pipertts/tts.hpp"

class ChatWidget : public QWidget {
 public:
  ChatWidget(QWidget *parent = nullptr) : QWidget(parent) {
    chatText = new QTextEdit(this);
    chatText->setReadOnly(true);
    chatText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QFont textFont = chatText->font();
    textFont.setPointSize(14);
    chatText->setFont(textFont);
    chatEdit = new QLineEdit(this);
		QFont editFont = chatEdit->font();
		editFont.setPointSize(16);
		chatEdit->setFont(editFont);
    chatEdit->setMinimumHeight(40);
		sendButton = new QPushButton(this);
		QFont buttonFont = sendButton->font();
		buttonFont.setPointSize(16);
		sendButton->setFont(buttonFont);
    sendButton->setMinimumHeight(40);
    sendButton->setText(QString::fromStdString("  Send"));
    connect(sendButton, &QPushButton::released, this, &ChatWidget::SendRequest);
    hchatLayout = new QHBoxLayout();
    hchatLayout->setSpacing(1);
    hchatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    hchatLayout->setContentsMargins(0, 0, 0, 0);
    hchatLayout->addWidget(chatEdit);
    hchatLayout->addWidget(sendButton);
    vchatLayout = new QVBoxLayout();
    vchatLayout->setSpacing(1);
    vchatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    vchatLayout->setContentsMargins(0, 0, 0, 0);
    vchatLayout->addWidget(chatText);
    chatLayout = new QGridLayout(this);
    chatLayout->setSpacing(1);
    chatLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->addLayout(vchatLayout, 0, 0, 1, 1);
    chatLayout->addLayout(hchatLayout, 1, 0, 1, 1);
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

 private slots:
  void UpdateChatContent(void) {
    std::string data;
    if (OllamaApi::GetRespone(data)) {
      chatContent += data;
      chatText->setMarkdown(QString::fromStdString(chatContent));
      chatText->moveCursor(QTextCursor::End);
			PiperTTS::SendRequest(data);
			spdlog::debug("[Qt]update chat content");
    }
  }

  void SendRequest(void) {
    std::string data;
    data = chatEdit->text().toStdString();
    chatEdit->clear();
    if (data.length() > 0) {
      OllamaApi::SendRequest(data);
      spdlog::debug("[Qt]send request");
    }
  }
};
