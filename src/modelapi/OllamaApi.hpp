#pragma once

#include <string>
#include <json/json.h>

class OllamaApi {
public:
    static void InitApi(std::string apiCurl, std::string modelName);
    static void SetApiUrl(std::string apiUrl);
    static void SetApiModel(std::string modelNmae);
    static void CallApi(const std::string content, std::string &result);
    static size_t OllamaWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

private:
    static std::string c_apiUrl;
    static std::string c_modelName;
    static std::string c_buffer;
    static Json::Value c_payload;
    static Json::Value c_messages;
};