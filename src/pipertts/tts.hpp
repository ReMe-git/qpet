#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>
#include <unordered_map>
#include "piper.hpp"
#include "../modelapi/TextParser.hpp"

class PiperTTS {
	public:
		static void Initialize(std::string zhModelPath, std::optional<piper::SpeakerId> zhSpeakerId, std::string enModelPath, std::optional<piper::SpeakerId> enSpeakerId, std::string eSpeakDataPath, bool useCuda);
		static void Release(void);
		static void Run(void);
		static bool GetRespone(std::vector<char> &respone);
		static void SendRequest(std::string request);
		static void LoadModel(enum LanguageType language, std::string modelPath, std::optional<piper::SpeakerId> speakerId);

	private:
		struct VoiceModel {
					piper::Voice model;
					std::optional<piper::SpeakerId> id;
		};
		static std::unordered_map<int, struct VoiceModel> models;
		static piper::PiperConfig c_config;
		static bool c_useCuda;
		static std::queue<std::vector<char>> responeQueue;
		static std::queue<std::string> requestQueue;
		static std::mutex responeLock;
		static std::mutex requestLock;
		static std::condition_variable hasRequest;
		static std::vector<char> c_buffer;
};
