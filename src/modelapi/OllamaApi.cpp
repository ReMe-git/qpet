#pragma once

class OllamaApi {
public:
    OllamaApi();
    ~OllamaApi();
    void SetApiAddress(const char* address);
    void SetModel(const char* model);
};