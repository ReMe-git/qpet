#include "TextParser.hpp"

#include <codecvt>
#include <locale>
#include <string>

static bool isChinese(wchar_t ch) {
  return (ch >= 0x4E00 && ch <= 0x9FFF) ||    // 基本汉字
         (ch >= 0x3400 && ch <= 0x4DBF) ||    // 扩展A区
         (ch >= 0x20000 && ch <= 0x2A6DF) ||  // 扩展B区
         (ch >= 0x2A700 && ch <= 0x2B73F) ||  // 扩展C区
         (ch >= 0x2B740 && ch <= 0x2B81F) ||  // 扩展D区
         (ch >= 0x2B820 && ch <= 0x2CEAF) ||  // 扩展E区
         (ch >= 0xF900 && ch <= 0xFAFF) ||    // 兼容汉字
         (ch >= 0x2F800 && ch <= 0x2FA1F);    // 兼容扩展
} // isChinese

static bool isEnglish(wchar_t ch) {
  return (ch >= 0x0041 && ch <= 0x005A) ||  // 大写字母
         (ch >= 0x0061 && ch <= 0x007A);    // 小写字母
} // isEnglish

static bool isEndofSentence(wchar_t ch) {
	return (ch == L',' || ch == L'.' || ch == L'?' || ch == L';' ||
			ch == L'!' || ch == L'。' || ch == L'，' || ch == L'：' ||
			ch == L'？' || ch == L'！' || ch == L'\n' || ch == L'\r');
} // isEndofSentence

static bool isHalf(wchar_t ch)
{
	return (ch >= 0x0000 && ch <= 0x007F);
} // isHalf

void TextParser::SplitText(void) {
  std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
  std::wstring buffer, content;
  int i, type = -1;
  content = converter.from_bytes(c_content);

  for (i = offset; i < content.length(); i++) {
		wchar_t wc = content.c_str()[i];
    if (isEnglish(wc)) {
      if (type != EN_US && type >= 0) {
        TextStruct newText(converter.to_bytes(buffer), type);
        textQueue.push(newText);
        type = EN_US;
        buffer.clear();
				offset = i;
        buffer += wc;
      } else {  // push buffer into queue
        type = EN_US;
        buffer += wc;
      }  // push char into buffer
    } else if (isChinese(wc)) {
      if (type != ZH_CN && type >= 0) {
        TextStruct newText(converter.to_bytes(buffer), type);
        textQueue.push(newText);
        type = ZH_CN;
        buffer.clear();
        buffer += wc;
				offset = i;
      } else {  // push buffer into queue
        type = ZH_CN;
        buffer += wc;
      }  // push char into buffer
    } else if (isEndofSentence(wc) && (buffer.length() > 0)) {
      if (type == -1)
        type = EN_US;
      TextStruct newText(converter.to_bytes(buffer), type);
      textQueue.push(newText);
      buffer.clear();
			offset = i;
    } else if (isHalf(wc)) {
      buffer += wc;
		} else {
    }  // check language type
  }  // range string
	c_content = converter.to_bytes(content.substr(offset));
	offset = 0;
}  // SplitText
