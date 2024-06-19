target_sources(${APP_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
  )

add_subdirectory(live2dwidget)
add_subdirectory(modelapi)
