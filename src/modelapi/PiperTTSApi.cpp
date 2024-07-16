#include "PiperTTSApi.hpp"

#include <curl/curl.h>
#include <json/json.h>

#include <condition_variable>
#include <mutex>
#include <queue>

#include "../live2dwidget/LAppDefine.hpp"
#include "spdlog/spdlog.h"
#include "TextParser.hpp"

using namespace LAppDefine;
using namespace TextParserSpace;

std::condition_variable PiperTTSApi::hasRequest;
std::mutex PiperTTSApi::requestLock;
std::mutex PiperTTSApi::responeLock;
std::queue<std::vector<char> > PiperTTSApi::responeQueue;
std::queue<std::string> PiperTTSApi::requestQueue;
std::string PiperTTSApi::c_apiUrl;
std::string PiperTTSApi::c_zhModel;
std::string PiperTTSApi::c_enModel;
std::vector<char> PiperTTSApi::c_buffer;

bool PiperTTSApi::GetRespone(std::vector<char> &respone) {
  responeLock.lock();
  if (!responeQueue.empty()) {
    respone = responeQueue.front();
    responeQueue.pop();
    responeLock.unlock();
    return true;
  } else {
    responeLock.unlock();
    return false;
  }
}  // GetRespone

void PiperTTSApi::SendRequest(std::string request) {
  requestLock.lock();
  requestQueue.push(request);
  spdlog::debug("[PiperTTS]send request");
  if (requestQueue.size() > 0) {
    hasRequest.notify_all();
    spdlog::debug("[PiperTTS]notify worker");
  }
  requestLock.unlock();
}  // PullRequest

size_t PiperTTSApi::PiperWriteCallback(void *contents, size_t size,
                                       size_t nmemb, void *userp) {
  std::vector<char> *readBuffer = static_cast<std::vector<char> *>(userp);
  char *responeBuffer = static_cast<char *>(contents);
  size_t totalSize = size * nmemb;
  readBuffer->insert(readBuffer->end(), responeBuffer,
                     responeBuffer + totalSize);
  return totalSize;
}  // PiperWriteCallback

void PiperTTSApi::CallApi(std::string input, int type) {
  Json::Value payloadJson;
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "accept: audio/x-wav");
    payloadJson["backend"] = "piper";
    payloadJson["input"] = input;
    payloadJson["model"] = type == EN_US ? c_enModel : c_zhModel;

    Json::StreamWriterBuilder writer;
    std::string payload = Json::writeString(writer, payloadJson);
    curl_easy_setopt(curl, CURLOPT_URL, c_apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PiperWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &c_buffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      spdlog::error("[PpierTTS]fail call api, error: {}", static_cast<int>(res));
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    spdlog::debug("[PiperTTS]call pipertts api, return {}", static_cast<int>(res));
  }
  curl_global_cleanup();

  responeLock.lock();
  responeQueue.push(c_buffer);
  c_buffer.clear();
  responeLock.unlock();
}  // CallApi

void PiperTTSApi::InitApi(std::string apiUrl, std::string zhModel,
                          std::string enModel) {
  c_apiUrl = apiUrl;
  c_zhModel = zhModel;
  c_enModel = enModel;
  c_buffer.clear();
}  // InitApi

void PiperTTSApi::Run(void) {
  std::string requestContent;
	TextParser parser(requestContent);
  
	while (true) {
    std::unique_lock<std::mutex> rlk(requestLock);
		
		if (requestQueue.empty() && parser.isEmpty()) {
      spdlog::debug("[PiperTTS]api wait request");
      hasRequest.wait(rlk);
		} else if (!requestQueue.empty()) {
			requestContent = requestQueue.front();
			requestQueue.pop();
			parser.AppendText(requestContent);
			parser.SplitText();
			spdlog::debug("[PiperTTS]api get request");
			requestLock.unlock();
		}

  	TextStruct text;	
		if (parser.GetText(text)) {
			std::string content; 
			text.GetContent(content);
      CallApi(content, text.GetLanguageType());
      spdlog::debug("[PiperTTS]deal content {}", content.c_str());
    }
  }
}  // Run
