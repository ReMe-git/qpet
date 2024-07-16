#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>
#include "piper.hpp"

class PiperTTS {
	public:
		static void Initialize(std::string modelPath, std::optional<piper::SpeakerId> speakerId, std::string eSpeakDataPath, bool useCuda);
		static void Release(void);
		static void Run(void);
		static bool GetRespone(std::vector<char> &respone);
		static void SendRequest(std::string request);

	private:
		static piper::Voice c_voice;
		static piper::PiperConfig c_config;
		static std::optional<piper::SpeakerId> c_speakerId;
		static std::queue<std::vector<char>> responeQueue;
		static std::queue<std::string> requestQueue;
		static std::mutex responeLock;
		static std::mutex requestLock;
		static std::condition_variable hasRequest;
		static std::vector<char> c_buffer;
};
