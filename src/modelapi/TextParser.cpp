#include "TextParser.hpp"

#include <codecvt>
#include <locale>
#include <string>
#include <iostream>

// 判断是否是中文字符
static bool isChinese(wchar_t ch) {
  return (ch >= 0x4E00 && ch <= 0x9FFF) ||    // 基本汉字
         (ch >= 0x3400 && ch <= 0x4DBF) ||    // 扩展A区
         (ch >= 0x20000 && ch <= 0x2A6DF) ||  // 扩展B区
         (ch >= 0x2A700 && ch <= 0x2B73F) ||  // 扩展C区
         (ch >= 0x2B740 && ch <= 0x2B81F) ||  // 扩展D区
         (ch >= 0x2B820 && ch <= 0x2CEAF) ||  // 扩展E区
         (ch >= 0xF900 && ch <= 0xFAFF) ||    // 兼容汉字
         (ch >= 0x2F800 && ch <= 0x2FA1F);    // 兼容扩展
}

// 判断是否是英文字符
static bool isEnglish(wchar_t ch) {
  return (ch >= 0x0041 && ch <= 0x005A) ||  // 大写字母
         (ch >= 0x0061 && ch <= 0x007A);    // 小写字母
}

// 判断是否是全角标点符号
static bool isFullwidthPunctuation(wchar_t ch) {
  return (ch >= 0xFF01 &&
          ch <= 0xFF0F) ||  // 全角标点符号 ! " # $ % & ' ( ) * + , - . /
         (ch >= 0xFF1A && ch <= 0xFF1F) ||  // 全角标点符号 : ; < = > ? @
         (ch >= 0xFF3B && ch <= 0xFF3F) ||  // 全角标点符号 [ \ ] ^ _ `
         (ch >= 0xFF5B && ch <= 0xFF60) ||  // 全角标点符号 { | } ~
         (ch >= 0x3000 && ch <= 0x303F);    // CJK 标点符号
}

// 判断是否是半角标点符号
static bool isHalfwidthPunctuation(wchar_t ch) {
  return (ch >= 0x0021 &&
          ch <= 0x002F) ||  // 半角标点符号 ! " # $ % & ' ( ) * + , - . /
         (ch >= 0x003A && ch <= 0x003F) ||  // 半角标点符号 : ; < = > ? @
         (ch >= 0x005B && ch <= 0x005F) ||  // 半角标点符号 [ \ ] ^ _ `
         (ch >= 0x007B && ch <= 0x007E);    // 半角标点符号 { | } ~
}

// 判断是否是阿拉伯数字
static bool isArabicNumber(wchar_t ch) {
  return (ch >= 0x0030 && ch <= 0x0039);  // 阿拉伯数字 0-9
}

// 判断是否是换行符
static bool isNewline(wchar_t ch) {
  return ch == L'\n' || ch == L'\r';  // 换行符 \n 和 \r
}

void TextParser::SplitTextByLanguageType(void) {
  std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
  std::wstring buffer, content;
  int type = -1;
  content = converter.from_bytes(c_content);

  for (wchar_t wc : content) {
    if (isEnglish(wc) || isHalfwidthPunctuation(wc)) {
      if (type != EN_US && type >= 0) {
        TextStruct newText(converter.to_bytes(buffer), type);
        textQueue.push(newText);
        type = EN_US;
        buffer.clear();
        buffer += wc;
      } else {  // push buffer into queue
        type = EN_US;
        buffer += wc;
      }  // push char into buffer
    } else if (isChinese(wc) || isFullwidthPunctuation(wc)) {
      if (type != ZH_CN && type >= 0) {
        TextStruct newText(converter.to_bytes(buffer), type);
        textQueue.push(newText);
        type = ZH_CN;
        buffer.clear();
        buffer += wc;
      } else {  // push buffer into queue
        type = ZH_CN;
        buffer += wc;
      }  // push char into buffer
    } else if (isNewline(wc) && (buffer.length() > 0)) {
      TextStruct newText(converter.to_bytes(buffer), type);
      textQueue.push(newText);
      buffer.clear();
      type = -1;
    } else {
      buffer += wc;
    }  // check language type

  }  // range string
  TextStruct newText(converter.to_bytes(buffer), type);
  textQueue.push(newText);
  buffer.clear();
}  // SplitTextByLanguageType

void TextParser::SPlitTextByPunctuation(void) {
  std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
  std::wstring buffer, content; 
  content = converter.from_bytes(c_content);
  int i = offset;

  for (; i < content.length(); i++) {
    wchar_t wc = content.c_str()[i];
    if (wc == L',' || wc == L'.' || wc == L'?' ||
     wc == L'。' || wc == L'，' || wc == L'？' || wc == L'\n' || wc == L'\r') {
      if (buffer.length() > 0) {
        buffer += wc;
        TextStruct newText(converter.to_bytes(buffer), -1);
        textQueue.push(newText);
      }
      offset = i;
      buffer.clear();
    } else {
      buffer += wc;
    }
  }
} // SPlitTextByPunctuation
