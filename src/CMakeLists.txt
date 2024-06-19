target_sources(${APP_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/ui.hpp
  )

add_subdirectory(live2dwidget)
