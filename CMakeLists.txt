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
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_LINUX_GL)
# Link libraries to framework.

target_link_libraries(Framework
	Live2DCubismCore
	Qt6::OpenGL
)

# Find opengl libraries.
find_package(Qt6 REQUIRED COMPONENTS 
	Core
	Gui
	Widgets
	OpenGLWidgets
	OpenGL
	Multimedia
)
find_package(CURL)
find_package(jsoncpp)

# Make executable app.
add_executable(${APP_NAME})
# Add source files.
add_subdirectory(src)
# Link libraries to app.
target_link_libraries(${APP_NAME}
  Framework
  CURL::libcurl
  jsoncpp
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
	Qt6::OpenGLWidgets
	Qt6::OpenGL
	Qt6::Multimedia
)

# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${STB_PATH})

# Copy resource directory to build directory.
add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/Resources
)
