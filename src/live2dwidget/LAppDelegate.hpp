/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <string>
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
#include <QMouseEvent>
#include <QOpenGLFunctions>

#include "LAppAllocator.hpp"
#include "LAppWavFileHandler.hpp"
#include "Live2dWidget.hpp"
#include "Type/csmVector.hpp"

class LAppView;
class LAppTextureManager;

/**
 * @brief   アプリケーションクラス。
 *   Cubism3の管理を行う。
 */
class LAppDelegate {
 public:
  /**
   * @brief   クラスのインスタンス（シングルトン）を返す。<br>
   *           インスタンスが生成されていない場合は内部でインスタンを生成する。
   *
   * @return  クラスのインスタンス
   */
  static LAppDelegate *GetInstance();

  /**
   * @brief   クラスのインスタンス（シングルトン）を解放する。
   *
   */
  static void ReleaseInstance();

  /**
   * @brief   APPに必要なものを初期化する。
   */
  bool Initialize(Live2dWidget *window);

  /**
   * @brief   解放する。
   */
  void Release();

  /**
   * @brief   実行処理。
   */
  void Run();

  /**
   * @brief シェーダーを登録する。
   */
  GLuint CreateShader();

  /**
   * @brief   Window情報を取得する。
   */
  Live2dWidget *GetWindow() { return _window; }

  /**
   * @brief   View情報を取得する。
   */
  LAppView *GetView() { return _view; }

  /**
   * @brief   アプリケーションを終了するかどうか。
   */
  bool GetIsEnd() { return _isEnd; }

  /**
   * @brief   アプリケーションを終了する。
   */
  void AppEnd() { _isEnd = true; }

  /**
   * @brief   アプリケーションの実行パスを設定する。
   */
  void SetExecuteAbsolutePath();

  /**
   * @brief   アプリケーションの実行パスを取得する。
   */
  std::string GetExecuteAbsolutePath() { return _executeAbsolutePath; }

  /**
   * @brief   テクスチャマネージャーを取得する。
   */
  LAppTextureManager *GetTextureManager() { return _textureManager; }
  LAppWavFileHandler *GetWavFileHandler() { return _wavFileHandler; }

  Csm::csmFloat32 GetVolume(void) { return volume; }
  void Resize(int w, int h);
  void GetSize(int *w, int *h) {
    *w = _window->width();
    *h = _window->height();
  }

  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

 private:
  /**
   * @brief   コンストラクタ
   */
  LAppDelegate();

  /**
   * @brief   デストラクタ
   */
  ~LAppDelegate();

  /**
   * @brief   Cubism3の初期化
   */
  void InitializeCubism();

  int Initialized;
  LAppAllocator _cubismAllocator;              ///< Cubism3 Allocator
  Csm::CubismFramework::Option _cubismOption;  ///< Cubism3 Option
  Live2dWidget *_window;                       ///< OpenGL ウィンドウ
  LAppView *_view;                             ///< View情報
  bool _captured;                              ///< クリックしているか
  float _mouseX;                               ///< マウスX座標
  float _mouseY;                               ///< マウスY座標
  bool _isEnd;                                 ///< APP終了しているか
  LAppTextureManager *_textureManager;  ///< テクスチャマネージャー
  std::string _executeAbsolutePath;  ///< アプリケーションの実行パス
  LAppWavFileHandler *_wavFileHandler;
  Csm::csmFloat32 volume;
  int _windowWidth;   ///< Initialize関数で設定したウィンドウ幅
  int _windowHeight;  ///< Initialize関数で設定したウィンドウ高さ
};
