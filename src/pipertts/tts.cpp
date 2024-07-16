#include "tts.hpp"
#include <spdlog/spdlog.h>
#include "piper.hpp"
#include "../modelapi/TextParser.hpp"

piper::PiperConfig PiperTTS::c_config;
piper::Voice PiperTTS::c_voice;
std::optional<piper::SpeakerId> PiperTTS::c_speakerId;
std::queue<std::vector<char>> PiperTTS::responeQueue;
std::queue<std::string> PiperTTS::requestQueue;
std::mutex PiperTTS::responeLock;
std::mutex PiperTTS::requestLock;
std::condition_variable PiperTTS::hasRequest;
std::vector<char> PiperTTS::c_buffer;


bool PiperTTS::GetRespone(std::vector<char> &respone) {
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

void PiperTTS::SendRequest(std::string request) {
  requestLock.lock();
  requestQueue.push(request);
  spdlog::debug("[PiperTTS]send request");
  if (requestQueue.size() > 0) {
    hasRequest.notify_all();
    spdlog::debug("[PiperTTS]notify worker");
  }
  requestLock.unlock();
}  // PullRequest

void PiperTTS::Initialize(std::string modelPath, std::optional<piper::SpeakerId> speakerId, std::string eSpeakDataPath, bool useCuda) {
	c_config.eSpeakDataPath = eSpeakDataPath;
	c_speakerId = speakerId;
	initialize(c_config);
	loadVoice(c_config, modelPath, modelPath + ".json", c_voice, c_speakerId, useCuda);
} // InitPiperTTS

void PiperTTS::Release(void) {
	piper::terminate(c_config);
}

void PiperTTS::Run(void) {
	std::string request;
	TextParser parser(request);

	while(true) {
		std::unique_lock<std::mutex> rlk(requestLock);
		
		if(requestQueue.empty() && parser.isEmpty()) {
			hasRequest.wait(rlk);
			spdlog::debug("[PiperTTS]wait request");
		} else if (!requestQueue.empty()){
			request = requestQueue.front();
			requestQueue.pop();
			parser.AppendText(request);
			parser.SplitText();
			spdlog::debug("[PiperTTS]api get request");
			requestLock.unlock();
		}

  		TextStruct text;	
		piper::SynthesisResult res;
		if (parser.GetText(text)) {
			std::string content; 
			text.GetContent(content);
			piper::textToWavBuffer(c_config, c_voice, content, c_buffer, res);
			if(!c_buffer.empty()) {
				responeLock.lock();
				responeQueue.push(c_buffer);
				responeLock.unlock();
			}
			c_buffer.clear();
		}
	}

} // Run
