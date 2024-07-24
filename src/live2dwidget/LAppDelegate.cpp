/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"

#include <libgen.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <QOpenGLFunctions>

#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppPal.hpp"
#include "LAppTextureManager.hpp"
#include "LAppView.hpp"
#include "LAppWavFileHandler.hpp"
#include "OpenGLExtraFunctions.hpp"
#include "spdlog/spdlog.h"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
LAppDelegate *s_instance = NULL;
}

LAppDelegate *LAppDelegate::GetInstance() {
  if (s_instance == NULL) {
    s_instance = new LAppDelegate();
  }

  return s_instance;
}

void LAppDelegate::ReleaseInstance() {
  if (s_instance != NULL) {
    delete s_instance;
  }

  s_instance = NULL;
}

bool LAppDelegate::Initialize(Live2dWidget *window) {
	spdlog::debug("[Live2D] START");

  _window = window;

  // テクスチャサンプリング設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // 透過設定
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // ウィンドウサイズ記憶
  int width, height;
  _windowWidth = width;
  _windowHeight = height;
  glViewport(0, 0, _windowWidth, _windowHeight);

  // AppViewの初期化
  _view->Initialize();

  // Cubism3の初期化
  InitializeCubism();

  SetExecuteAbsolutePath();

  // load model
  LAppLive2DManager::GetInstance();

  // load sprite
  _view->InitializeSprite();

  Initialized = true;
  return GL_TRUE;
}

void LAppDelegate::Release() {
  delete _textureManager;
  delete _view;
  delete _wavFileHandler;

  // リソースを解放
  LAppLive2DManager::ReleaseInstance();

  // Cubism3の解放
  CubismFramework::Dispose();
}

void LAppDelegate::Run() {
  if (!Initialized) return;

  _window->clear();
  // 時間更新
  LAppPal::UpdateTime();
  // 描画更新
  _view->Render();
}

void LAppDelegate::Resize(int w, int h) {
  _windowWidth = w;
  _windowHeight = h;

  // AppViewの初期化
  _view->Initialize();
  // スプライトサイズを再設定
  _view->ResizeSprite();
  // ビューポート変更
  OpenGLExtraFunctions::GetInstance()->glViewport(0, 0, _windowWidth,
                                                  _windowHeight);
}

LAppDelegate::LAppDelegate()
    : _cubismOption(),
      _window(NULL),
      _captured(false),
      _mouseX(0.0f),
      _mouseY(0.0f),
      _isEnd(false),
      _windowWidth(0),
      _windowHeight(0) {
  _executeAbsolutePath = "";
  _view = new LAppView();
  _textureManager = new LAppTextureManager();
  _wavFileHandler = new LAppWavFileHandler();
}

LAppDelegate::~LAppDelegate() {}

void LAppDelegate::InitializeCubism() {
  // setup cubism
  _cubismOption.LogFunction = LAppPal::PrintMessage;
  _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
  Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

  // Initialize cubism
  CubismFramework::Initialize();

  // default proj
  CubismMatrix44 projection;

  LAppPal::UpdateTime();
}

GLuint LAppDelegate::CreateShader() {
  // バーテックスシェーダのコンパイル
  GLuint vertexShaderId =
      OpenGLExtraFunctions::GetInstance()->glCreateShader(GL_VERTEX_SHADER);
  const char *vertexShader =
      "#version 120\n"
      "attribute vec3 position;"
      "attribute vec2 uv;"
      "varying vec2 vuv;"
      "void main(void){"
      "    gl_Position = vec4(position, 1.0);"
      "    vuv = uv;"
      "}";
  OpenGLExtraFunctions::GetInstance()->glShaderSource(vertexShaderId, 1,
                                                      &vertexShader, NULL);
  OpenGLExtraFunctions::GetInstance()->glCompileShader(vertexShaderId);

  // フラグメントシェーダのコンパイル
  GLuint fragmentShaderId =
      OpenGLExtraFunctions::GetInstance()->glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragmentShader =
      "#version 120\n"
      "varying vec2 vuv;"
      "uniform sampler2D texture;"
      "uniform vec4 baseColor;"
      "void main(void){"
      "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
      "}";
  OpenGLExtraFunctions::GetInstance()->glShaderSource(fragmentShaderId, 1,
                                                      &fragmentShader, NULL);
  OpenGLExtraFunctions::GetInstance()->glCompileShader(fragmentShaderId);

  // プログラムオブジェクトの作成
  GLuint programId = OpenGLExtraFunctions::GetInstance()->glCreateProgram();
  OpenGLExtraFunctions::GetInstance()->glAttachShader(programId,
                                                      vertexShaderId);
  OpenGLExtraFunctions::GetInstance()->glAttachShader(programId,
                                                      fragmentShaderId);

  // リンク
  OpenGLExtraFunctions::GetInstance()->glLinkProgram(programId);

  OpenGLExtraFunctions::GetInstance()->glUseProgram(programId);

  return programId;
}

void LAppDelegate::SetExecuteAbsolutePath() {
  char path[1024];
  ssize_t len = readlink("/proc/self/exe", path, 1024 - 1);

  if (len != -1) {
    path[len] = '\0';
  }

  this->_executeAbsolutePath = dirname(path);
  this->_executeAbsolutePath += "/";
}

void LAppDelegate::mouseMoveEvent(QMouseEvent *event) {
  _mouseX = static_cast<float>(event->pos().x() * Live2dWidget::ratio);
  _mouseY = static_cast<float>(event->pos().y() * Live2dWidget::ratio);
  if (!_captured) {
    return;
  }
  if (_view == NULL) {
    return;
  }

  _view->OnTouchesMoved(_mouseX, _mouseY);
}

void LAppDelegate::mousePressEvent(QMouseEvent *event) {
  _mouseX = static_cast<float>(event->pos().x() * Live2dWidget::ratio);
  _mouseY = static_cast<float>(event->pos().y() * Live2dWidget::ratio);

  if (_view == NULL) {
    return;
  }

  _captured = true;
  _view->OnTouchesBegan(_mouseX, _mouseY);
}
void LAppDelegate::mouseReleaseEvent(QMouseEvent *event) {
  _mouseX = static_cast<float>(event->pos().x() * Live2dWidget::ratio);
  _mouseY = static_cast<float>(event->pos().y() * Live2dWidget::ratio);

  if (_view == NULL) {
    return;
  }

  if (_captured) {
    _captured = false;
    _view->OnTouchesEnded(_mouseX, _mouseY);
  }
}
