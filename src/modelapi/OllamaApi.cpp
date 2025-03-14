#include "OllamaApi.hpp"

#include <curl/curl.h>
#include <json/json.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

#include "../live2dwidget/LAppDefine.hpp"
#include "spdlog/spdlog.h"

using namespace LAppDefine;

std::condition_variable OllamaApi::hasRequest;
std::mutex OllamaApi::requestLock;
std::mutex OllamaApi::responeLock;
std::queue<std::string> OllamaApi::requestQueue;
std::queue<std::string> OllamaApi::responeQueue;
std::string OllamaApi::c_apiUrl;
std::string OllamaApi::c_modelName;
std::string OllamaApi::c_buffer;
std::string OllamaApi::c_content;
Json::Value OllamaApi::c_messages;
Json::Value OllamaApi::c_payload;

bool OllamaApi::GetRespone(std::string &respone) {
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

void OllamaApi::SendRequest(std::string request) {
  requestLock.lock();
  requestQueue.push(request);
	spdlog::debug("[Ollama]send request");
  if (requestQueue.size() > 0) {
    hasRequest.notify_all();
      spdlog::debug("[Ollama]notify worker");
  }
  requestLock.unlock();
}  // PullRequest

size_t OllamaApi::OllamaWriteCallback(void *contents, size_t size, size_t nmemb,
                                      void *userp) {
  std::string *readBuffer = static_cast<std::string *>(userp);
  Json::CharReaderBuilder readerBuilder;
  Json::CharReader *reader = readerBuilder.newCharReader();
  Json::Value respone, assistantContent;
  std::string errors;
  size_t totalSize = size * nmemb;

  if (!reader->parse(static_cast<char *>(contents),
                     static_cast<char *>(contents) + totalSize, &respone,
                     &errors)) {
    spdlog::debug("[Ollama] fail parse json");
		return totalSize;
  }
  delete reader;
	std::string appendContent;
  if (respone["message"]["content"] && respone["message"]["content"].isString()) {
		appendContent = respone["message"]["content"].asString();
	} else {
		spdlog::debug("[Ollama] inviaid value");
		return totalSize;
	}
  readBuffer->append(appendContent.c_str(), appendContent.length());
  responeLock.lock();
  responeQueue.push(appendContent);
  responeLock.unlock();
  if (respone["done"].asBool() == true) {
    assistantContent["content"] = c_buffer;
    assistantContent["role"] = "assistant";
    c_messages.append(assistantContent);
    c_buffer.clear();
  }
  return totalSize;
}  // OllamaWiriteCallBack

void OllamaApi::SetApiModel(std::string modelName) {
  c_modelName = modelName;
  c_payload["model"] = modelName;
}  // SetApiModel

void OllamaApi::SetApiUrl(std::string apiUrl) {
  c_apiUrl = apiUrl;
}  // SetApiUrl

void OllamaApi::CallApi(const std::string question) {
  Json::Value userContent;
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  if (curl) {
    struct curl_slist *headers = nullptr;

    userContent["content"] = question;
    userContent["role"] = "user";
    c_messages.append(userContent);
    c_payload["messages"] = c_messages;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    Json::StreamWriterBuilder writer;
    std::string payload = Json::writeString(writer, c_payload);
    curl_easy_setopt(curl, CURLOPT_URL, c_apiUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OllamaWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &c_buffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      spdlog::error("[Ollama] fail call ollama api, error: {}", static_cast<int>(res));
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  	spdlog::debug("[Ollama] call ollama api,return {}", static_cast<int>(res));
  }

  curl_global_cleanup();
}  // CallApi

void OllamaApi::InitApi(std::string apiCurl, std::string modelName) {
  c_apiUrl = apiCurl;
  c_modelName = modelName;
  c_payload["model"] = c_modelName;
  c_buffer.clear();
  c_content.clear();
}  // InitApi

void OllamaApi::Run(void) {
  std::string requestContent;
  while (true) {
    std::unique_lock<std::mutex> rlk(requestLock);
    while (requestQueue.empty()) {
      spdlog::debug("[Ollama] wait request");
      hasRequest.wait(rlk);
    }
    requestContent = requestQueue.front();
    requestQueue.pop();
    requestLock.unlock();
    spdlog::debug("[Ollama] get request");
    CallApi(requestContent);
  }
}  // Run
