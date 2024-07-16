cmake_minimum_required(VERSION 3.16)

# Set app name.
set(APP_NAME vpet)
# Set directory paths.
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CORE_PATH ${SDK_ROOT_PATH}/live2d/core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/live2d/framework)
set(STB_PATH ${SDK_ROOT_PATH}/stb)
set(RES_PATH ${SDK_ROOT_PATH}/res)

# Set project.
project(${APP_NAME})

# Define output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${APP_NAME})
# Set configuration (Release and Debug only).
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)
# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION ON)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 17)

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION ${CORE_PATH}/lib/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH}
	${CMAKE_CURRENT_BINARY_DIR}/framework
)

# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_LINUX_GL)

# Link libraries to framework.
target_link_libraries(Framework
	Live2DCubismCore
	Qt6::OpenGL
)

# Find qt.
find_package(Qt6 REQUIRED COMPONENTS 
	Core
	Gui
	Widgets
	OpenGLWidgets
	OpenGL
	Multimedia
)

# Find CURL
find_package(CURL)

# Find jsoncpp
find_package(jsoncpp)

include(ExternalProject)

# fmt
set(FMT_VERSION "10.0.0")
set(FMT_DIR "${CMAKE_CURRENT_BINARY_DIR}/fmt")

ExternalProject_Add(
	fmt_external
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}/fmt"
	URL "https://github.com/fmtlib/fmt/archive/refs/tags/${FMT_VERSION}.zip"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${FMT_DIR}
	CMAKE_ARGS -DFMT_TEST:BOOL=OFF  # Don't build all the tests
)

# spdlog
set(SPDLOG_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog")
set(SPDLOG_VERSION "1.12.0")

ExternalProject_Add(
	spdlog_external
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}/spdlog"
	URL "https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${SPDLOG_DIR}
)

# piper_phonemize
set(PIPER_PHONEMIZE_DIR "${CMAKE_CURRENT_BINARY_DIR}/piper_phonemize")

ExternalProject_Add(
	piper_phonemize_external
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}/piper_phonemize"
	URL "https://github.com/rhasspy/piper-phonemize/archive/refs/heads/master.zip"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${PIPER_PHONEMIZE_DIR}
)

# Make executable app.
add_executable(${APP_NAME})

add_dependencies(${APP_NAME} fmt_external)
add_dependencies(${APP_NAME} spdlog_external)
add_dependencies(${APP_NAME} piper_phonemize_external)

# Add source files.
add_subdirectory(src)

# Link libraries to app.
target_link_libraries(${APP_NAME}
  Framework
  jsoncpp
  CURL::libcurl
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::OpenGLWidgets
	Qt6::OpenGL
	Qt6::Multimedia
  fmt
  spdlog
  espeak-ng
  piper_phonemize
  onnxruntime
)

target_link_directories(${APP_NAME} PRIVATE
  ${FMT_DIR}/lib
	${SPDLOG_DIR}/lib
	${PIPER_PHONEMIZE_DIR}/lib
)

# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE
	${STB_PATH}
  ${FMT_DIR}/include
  ${SPDLOG_DIR}/include
  ${PIPER_PHONEMIZE_DIR}/include
)

# Copy resource directory to build directory.
add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/res
)

add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
			copy_directory ${PIPER_PHONEMIZE_DIR}/share/espeak-ng-data $<TARGET_FILE_DIR:${APP_NAME}>/res/piper/espeak-ng-data
)
