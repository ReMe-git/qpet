#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

class PiperTTSApi {
 public:
  static void InitApi(std::string apiUrl, std::string zhModel,
                      std::string enModel);
  static void CallApi(std::string content, int type);
  static bool GetRespone(std::vector<char> &respone);
  static void SendRequest(std::string request);
  static void Run(void);

 private:
  static size_t PiperWriteCallback(void *contents, size_t size, size_t nmemb,
                                   void *userp);
  static std::condition_variable hasRequest;
  static std::mutex requestLock;
  static std::mutex responeLock;
  static std::queue<std::string> requestQueue;
  static std::queue<std::vector<char> > responeQueue;
  static std::string c_apiUrl;
  static std::string c_zhModel;
  static std::string c_enModel;
  static std::vector<char> c_buffer;
};
