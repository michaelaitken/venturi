set(APP_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp"
)

set(APP_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Config.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp"
)

add_executable(${EXECUTABLE_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_link_libraries(
  ${EXECUTABLE_NAME}
  PRIVATE "venturi-core" "venturi-adapters"
)
