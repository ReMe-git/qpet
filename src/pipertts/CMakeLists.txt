target_sources(${APP_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/piper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/piper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/wavfile.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/utf8.h
	${CMAKE_CURRENT_SOURCE_DIR}/utf8/cpp11.h
	${CMAKE_CURRENT_SOURCE_DIR}/utf8/cpp17.h
	${CMAKE_CURRENT_SOURCE_DIR}/utf8/unchecked.h
	${CMAKE_CURRENT_SOURCE_DIR}/utf8/checked.h
	${CMAKE_CURRENT_SOURCE_DIR}/utf8/core.h
	${CMAKE_CURRENT_SOURCE_DIR}/tts.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/tts.cpp
)
