#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <json/json.h>


class OllamaApi {
public:
    static void InitApi(std::string apiCurl, std::string modelName);
    static void SetApiUrl(std::string apiUrl);
    static void SetApiModel(std::string modelNmae);
    static void CallApi(const std::string question);
    static size_t OllamaWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static bool GetRespone(std::string &resopne);
    static void SendRequest(std::string request);
    static void Run(void);

private:
    static std::mutex requestLock;
    static std::mutex responeLock;
    static std::condition_variable hasRequest;
    static std::queue<std::string> requestQueue;
    static std::queue<std::string> responeQueue;
    static std::string c_apiUrl;
    static std::string c_modelName;
    static std::string c_buffer;
    static std::string c_content;
    static Json::Value c_payload;
    static Json::Value c_messages;
};