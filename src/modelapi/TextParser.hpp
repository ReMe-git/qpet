#pragma once

#include <queue>
#include <string>

namespace TextParserSpace {
enum LanguageType { EN_US, ZH_CN };
};

using namespace TextParserSpace;

class TextStruct {
 public:
  TextStruct() {}
  TextStruct(std::string content, int type) {
    c_content = content;
    c_type = type;
  }

  void GetContent(std::string &str) { str = c_content; }

  int GetLanguageType(void) { return c_type; }

 private:
  std::string c_content;
  int c_type;
};

class TextParser {
 public:
  TextParser();
  TextParser(std::string content) { c_content = content; }
  void SetText(std::string content) { c_content = content; }
  void ParseText(void);
  bool GetText(TextStruct &ret) {
    if (!textQueue.empty()) {
      ret = textQueue.front();
      textQueue.pop();
      return true;
    } else {
      return false;
    }
  }

 private:
  std::string c_content;
  std::queue<TextStruct> textQueue;
};
