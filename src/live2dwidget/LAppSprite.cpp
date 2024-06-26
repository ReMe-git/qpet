/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at
 * https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"

#include "LAppDelegate.hpp"
#include "OpenGLExtraFunctions.hpp"

LAppSprite::LAppSprite(float x, float y, float width, float height,
                       GLuint textureId, GLuint programId)
    : _rect() {
  _rect.left = (x - width * 0.5f);
  _rect.right = (x + width * 0.5f);
  _rect.up = (y + height * 0.5f);
  _rect.down = (y - height * 0.5f);
  _textureId = textureId;

  // 何番目のattribute変数か
  _positionLocation = OpenGLExtraFunctions::GetInstance()->glGetAttribLocation(
      programId, "position");
  _uvLocation =
      OpenGLExtraFunctions::GetInstance()->glGetAttribLocation(programId, "uv");
  _textureLocation = OpenGLExtraFunctions::GetInstance()->glGetUniformLocation(
      programId, "texture");
  _colorLocation = OpenGLExtraFunctions::GetInstance()->glGetUniformLocation(
      programId, "baseColor");

  _spriteColor[0] = 1.0f;
  _spriteColor[1] = 1.0f;
  _spriteColor[2] = 1.0f;
  _spriteColor[3] = 1.0f;
}

LAppSprite::~LAppSprite() {}

void LAppSprite::Render() const {
  // 画面サイズを取得する
  int maxWidth, maxHeight;
  LAppDelegate::GetInstance()->GetSize(&maxWidth, &maxHeight);

  if (maxWidth == 0 || maxHeight == 0) {
    return;
  }

  const GLfloat uvVertex[] = {
      1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  // attribute属性を有効にする
  OpenGLExtraFunctions::GetInstance()->glEnableVertexAttribArray(
      _positionLocation);
  OpenGLExtraFunctions::GetInstance()->glEnableVertexAttribArray(_uvLocation);

  // uniform属性の登録
  OpenGLExtraFunctions::GetInstance()->glUniform1i(_textureLocation, 0);

  // 頂点データ
  float positionVertex[] = {
      (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f)};

  // attribute属性を登録
  OpenGLExtraFunctions::GetInstance()->glVertexAttribPointer(
      _positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
  OpenGLExtraFunctions::GetInstance()->glVertexAttribPointer(
      _uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

  OpenGLExtraFunctions::GetInstance()->glUniform4f(
      _colorLocation, _spriteColor[0], _spriteColor[1], _spriteColor[2],
      _spriteColor[3]);

  // モデルの描画
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void LAppSprite::RenderImmidiate(GLuint textureId,
                                 const GLfloat uvVertex[8]) const {
  // 画面サイズを取得する
  int maxWidth, maxHeight;
  LAppDelegate::GetInstance()->GetSize(&maxWidth, &maxHeight);

  if (maxWidth == 0 || maxHeight == 0) {
    return;
  }

  // attribute属性を有効にする
  OpenGLExtraFunctions::GetInstance()->glEnableVertexAttribArray(
      _positionLocation);
  OpenGLExtraFunctions::GetInstance()->glEnableVertexAttribArray(_uvLocation);

  // uniform属性の登録
  OpenGLExtraFunctions::GetInstance()->glUniform1i(_textureLocation, 0);

  // 頂点データ
  float positionVertex[] = {
      (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
      (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f),
      (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f)};

  // attribute属性を登録
  OpenGLExtraFunctions::GetInstance()->glVertexAttribPointer(
      _positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
  OpenGLExtraFunctions::GetInstance()->glVertexAttribPointer(
      _uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

  OpenGLExtraFunctions::GetInstance()->glUniform4f(
      _colorLocation, _spriteColor[0], _spriteColor[1], _spriteColor[2],
      _spriteColor[3]);

  // モデルの描画
  glBindTexture(GL_TEXTURE_2D, textureId);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

bool LAppSprite::IsHit(float pointX, float pointY) const {
  // 画面サイズを取得する
  int maxWidth, maxHeight;
  LAppDelegate::GetInstance()->GetSize(&maxWidth, &maxHeight);

  if (maxWidth == 0 || maxHeight == 0) {
    return false;
  }
  // Y座標は変換する必要あり
  float y = maxHeight - pointY;

  return (pointX >= _rect.left && pointX <= _rect.right && y <= _rect.up &&
          y >= _rect.down);
}

void LAppSprite::SetColor(float r, float g, float b, float a) {
  _spriteColor[0] = r;
  _spriteColor[1] = g;
  _spriteColor[2] = b;
  _spriteColor[3] = a;
}

void LAppSprite::ResetRect(float x, float y, float width, float height) {
  _rect.left = (x - width * 0.5f);
  _rect.right = (x + width * 0.5f);
  _rect.up = (y + height * 0.5f);
  _rect.down = (y - height * 0.5f);
}
