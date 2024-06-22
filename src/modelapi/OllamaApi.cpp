#include <mutex>
#include <queue>
#include <condition_variable>
#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <thread>
#include "OllamaApi.hpp"
#include "../live2dwidget/LAppDefine.hpp"
#include "../live2dwidget/LAppPal.hpp"

using namespace LAppDefine;

std::condition_variable OllamaApi::hasRequest;
std::mutex OllamaApi::requestLock;
std::mutex OllamaApi::responeLock;
std::queue<std::string> OllamaApi::requestQueue;
std::queue<std::string> OllamaApi::responeQueue;
std::string OllamaApi::c_apiUrl = "";
std::string OllamaApi::c_modelName = "";
std::string OllamaApi::c_buffer = "";
std::string OllamaApi::c_content = "";
Json::Value OllamaApi::c_messages;
Json::Value OllamaApi::c_payload;

bool OllamaApi::GetRespone(std::string &respone)
{
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
} //GetRespone

void OllamaApi::SendRequest(std::string request)
{
    requestLock.lock();
    requestQueue.push(request);
    if (DebugLogEnable) {
        LAppPal::PrintLogLn("[API] send request");
    }
    if (requestQueue.size() > 0) {
        hasRequest.notify_all();
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[API]notify worker");
        }
    }
    requestLock.unlock();
} //PullRequest

size_t OllamaApi::OllamaWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* readBuffer = static_cast<std::string*>(userp);
    Json::CharReaderBuilder readerBuilder;
    Json::CharReader* reader = readerBuilder.newCharReader();
    Json::Value respone, assistantContent;
    std::string errors;
    size_t totalSize = size * nmemb;

    if (!reader->parse(static_cast<char *>(contents), static_cast<char *>(contents) + totalSize, &respone, &errors)) {
        LAppPal::PrintLogLn("[API]failed parse json");
    }
    delete reader;
    std::string appendContent = respone["message"]["content"].asString();
    readBuffer->append(appendContent.c_str(), appendContent.length());
    responeLock.lock();
    responeQueue.push(appendContent);
    responeLock.unlock();
    if (respone["done"].asBool() == true) {
        assistantContent["content"] = c_buffer;
        assistantContent["role"] = "assistant";
        c_messages.append(assistantContent);
        c_buffer = "";
    }
    return totalSize;
} //OllamaWiriteCallBack

void OllamaApi::SetApiModel(std::string modelName)
{
    c_modelName = modelName;
    c_payload["model"] = modelName;
} //SetApiModel

void OllamaApi::SetApiUrl(std::string apiUrl)
{
    c_apiUrl = apiUrl;
} //SetApiUrl

void OllamaApi::CallApi(const std::string question) {
    Json::Value userContent;
    CURL* curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = nullptr;
        
        userContent["content"] = question;
        userContent["role"] = "user";
        c_messages.append(userContent);
        c_payload["messages"]= c_messages;
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
            if (DebugLogEnable) {
                LAppPal::PrintLogLn("[API]failed call ollama api, error: %d", res);
            }
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[API]called ollama api");
        }
    }

    curl_global_cleanup();
} //CallApi

void OllamaApi::InitApi(std::string apiCurl, std::string modelName)
{
    c_apiUrl = apiCurl;
    c_modelName = modelName;
    c_payload["model"] = c_modelName;
    c_buffer = "";
    c_content = "";
} //InitApi

void OllamaApi::Run(void)
{
    std::string requestContent;
    while (true) {
        std::unique_lock<std::mutex> rlk(requestLock);
        while (requestQueue.empty()) {
            if (DebugLogEnable) {
                LAppPal::PrintLogLn("[API]wait request");
            }
            hasRequest.wait(rlk);  
        }
        requestContent = requestQueue.front();
        requestQueue.pop();
        requestLock.unlock();
        if (DebugLogEnable) {
            LAppPal::PrintLogLn("[API]get request");
        }
        CallApi(requestContent);
    }
}